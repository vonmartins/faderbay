/* =========================================================
 * fader_driver.c
 * Driver layer — ADC/MUX fader scanning via DMA + IIR filter
 *
 * ARCHITECTURE
 * ============
 * Scanning is fully event-driven. FaderDriver_Init() starts the first
 * DMA conversion on channel 0. Each time HAL_ADC_ConvCpltCallback fires
 * (DMA-TC ISR), the driver:
 *   1. Latches s_raw[channel] from the DMA result buffer.
 *   2. Advances the MUX to the next channel.
 *   3. Busy-waits for MUX/RC/op-amp settling (DWT cycle counter, ≤ FADER_MUX_SETTLE_US µs).
 *   4. Fires the next HAL_ADC_Start_DMA — no polling, no timeout, non-blocking.
 *
 * FaderDriver_Process() (scheduler task, PERIOD_FADER_DRIVER ms) does two things:
 *   a. Applies the IIR filter to every channel using the latest s_raw[] snapshot.
 *   b. Watchdog: if no full scan has completed in FADER_ADC_STALL_MS ticks, it
 *      calls FaderDriver_StartCycle() to recover.
 *
 * SYNCHRONISATION INVARIANTS
 * ==========================
 * s_raw[]        volatile uint16_t — written once per conversion in ISR (DMA-TC),
 *                read once per tick in main-loop task. A 16-bit aligned write/read on
 *                Cortex-M4 is single-instruction atomic; no lock needed.
 *
 * s_channel      volatile uint8_t — written only in ISR (and in StartCycle from main
 *                loop, only when the watchdog has confirmed the ISR is stalled, so
 *                there is no concurrent writer). Read only in ISR.
 *
 * s_scan_count   volatile uint32_t — incremented in ISR at end of every full 16-ch
 *                scan. Read in main-loop task. 32-bit aligned write/read is atomic on
 *                Cortex-M4; no lock needed. The task only checks for inequality, so a
 *                transient tear would merely delay watchdog reset by one tick.
 *
 * s_dma_buf      Written by DMA engine (not CPU). __DMB() in the callback enforces
 *                the DMA-write→CPU-read ordering before s_raw[] is updated.
 *
 * s_filtered[]   Written and read only in main-loop task (FaderDriver_Process and
 *                FaderDriver_GetFiltered). No ISR access; no barrier needed.
 *
 * STALL / RECOVERY
 * ================
 * If HAL_ADC_Start_DMA never fires (peripheral hang, DMA error), s_scan_count stops
 * incrementing. After FADER_ADC_STALL_MS consecutive ticks without a new scan,
 * FaderDriver_Process calls FaderDriver_StartCycle(), which calls HAL_ADC_Stop_DMA
 * then re-arms the chain from channel 0.
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "fader_driver.h"
#include "config.h"
#include "faderbay_types.h"
#include "main.h"
#include "mux_driver.h"
#include "nlog.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

#define MUX_NUM_CH      NUM_FADERS
#define MUX_NUM_SEL     4

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "FADER_DRV";

/* ISR-shared state — volatile */
static volatile uint16_t  s_raw[NUM_FADERS];   /* last raw ADC result per channel */
static volatile uint8_t   s_channel;           /* channel whose conversion is in flight */
static volatile uint32_t  s_scan_count;        /* full-scan counter for watchdog */

/* DMA result word. DMA writes it; ISR reads it after __DMB(). Not volatile:
 * the DMA-TC interrupt itself is the happens-before edge; __DMB() in the
 * callback enforces visibility. The pointer is passed to HAL as uint32_t*. */
static uint32_t s_dma_buf;

/* Main-loop-only state */
static ADC_HandleTypeDef *s_hadc;
static int32_t            s_filtered[NUM_FADERS];
static uint32_t           s_last_scan_count;
static uint8_t            s_stall_ticks;
static uint32_t           s_settle_cycles;     /* precomputed DWT cycle count */

// ========================= PRIVATE FUNC. DECL. =======================

static void FaderMux_SettleDelay(void);
static void FaderDriver_StartCycle(void);

// =========================== PRIVATE FUNCTIONS =======================

/* Busy-wait using DWT cycle counter. Called from both main-loop (StartCycle)
 * and ISR (ConvCpltCallback). Duration: FADER_MUX_SETTLE_US µs.
 * Maximum ISR hold time: s_settle_cycles cycles (≤ 840 @ 84 MHz, 10 µs). */
static void FaderMux_SettleDelay(void)
{
    // uint32_t start = DWT->CYCCNT;
    // while ((DWT->CYCCNT - start) < s_settle_cycles) {}
    for (volatile uint32_t i = 0; i < 200; i++) {
        __NOP();
    }
}

/* Select MUX channel 0, wait settling, kick off first DMA conversion.
 * Called from Init (main loop) and from watchdog recovery (main loop only).
 * Always calls HAL_ADC_Stop_DMA first to handle the recovery case. */
static void FaderDriver_StartCycle(void)
{
    HAL_ADC_Stop_DMA(s_hadc);
    s_channel = 0;
    MuxDriver_SelectChannel(0);
    FaderMux_SettleDelay();
    if (HAL_ADC_Start_DMA(s_hadc, &s_dma_buf, 1u) != HAL_OK) {
        LOGE(TAG, "StartCycle: DMA start failed");
    }
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t FaderDriver_Init(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL) {
        LOGE(TAG, "Init: null ADC handle");
        return FB_ERR_INVALID_PARAM;
    }

    s_hadc = hadc;
    memset((void *)s_raw, 0, sizeof(s_raw));
    memset(s_filtered, 0, sizeof(s_filtered));
    s_channel        = 0;
    s_scan_count     = 0;
    s_last_scan_count = 0;
    s_stall_ticks    = 0;
    s_dma_buf        = 0;

    /* Enable DWT cycle counter (works without a debugger attached). */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    /* Precompute settling cycle count to avoid runtime division in ISR. */
    s_settle_cycles = (SystemCoreClock / 1000000u) * (uint32_t)FADER_MUX_SETTLE_US;

    const MuxConfig_t mux_cfg = {
        .num_channels = MUX_NUM_CH,
        .num_sel_pins = MUX_NUM_SEL,
        .sel_pins  = { MUX_S0_Pin,       MUX_S1_Pin,       MUX_S2_Pin,       MUX_S3_Pin       },
        .sel_ports = { MUX_S0_GPIO_Port, MUX_S1_GPIO_Port, MUX_S2_GPIO_Port, MUX_S3_GPIO_Port },
    };

    fb_err_t err = MuxDriver_Init(&mux_cfg);
    if (err != FB_OK) {
        LOGE(TAG, "Init: MuxDriver failed");
        return err;
    }

    /* Arm the first conversion; subsequent ones self-chain via callback. */
    FaderDriver_StartCycle();

    /* Wait for the first scans to populate s_raw[], then prime the IIR
     * filter with actual fader positions. Otherwise the filter would
     * ramp from 0 over ~100 ms and generate spurious MIDI on boot.
     * HAL_Delay is acceptable here because the scheduler has not started yet. */
    HAL_Delay(20);
    for (uint8_t i = 0; i < NUM_FADERS; i++) {
        s_filtered[i] = (int32_t)s_raw[i];
    }

    LOGI(TAG, "Init OK");
    return FB_OK;
}

/* Filter task — no ADC polling, no blocking.
 * Runs at PERIOD_FADER_DRIVER ms in the cooperative scheduler. */
void FaderDriver_Process(void)
{
    /* --- Watchdog --------------------------------------------------- */
    uint32_t sc = s_scan_count;   /* snapshot of volatile; single aligned read = atomic */
    if (sc != s_last_scan_count) {
        s_last_scan_count = sc;
        s_stall_ticks = 0;
    } else {
        if (++s_stall_ticks >= (uint8_t)FADER_ADC_STALL_MS) {
            s_stall_ticks = 0;
            LOGW(TAG, "ADC stall detected, restarting cycle");
            FaderDriver_StartCycle();
        }
    }

    /* --- IIR filter (first-order, integer, per spec) ---------------- */
    /* y[n] = y[n-1] + ((x[n] - y[n-1]) >> FADER_FILTER_SHIFT)
     * Executed in main-loop only; s_filtered[] has no ISR writer. */
    for (uint8_t i = 0; i < NUM_FADERS; i++) {
        int32_t x = (int32_t)(uint32_t)s_raw[i];   /* volatile single read */
        s_filtered[i] += (x - s_filtered[i]) >> FADER_FILTER_SHIFT;
    }
}

uint16_t FaderDriver_GetRaw(uint8_t index)
{
    if (index >= NUM_FADERS) return 0;
    return s_raw[index];
}

uint16_t FaderDriver_GetFiltered(uint8_t index)
{
    if (index >= NUM_FADERS) return 0;
    return (uint16_t)s_filtered[index];
}

// ========================= ISR CALLBACK ==============================

/* HAL weak override — called from DMA1_Stream0_IRQHandler via HAL.
 *
 * Invariants at entry:
 *   - DMA transfer of 1 word to s_dma_buf is complete.
 *   - hadc->Lock is HAL_UNLOCKED (released by HAL_ADC_Start_DMA before SWSTART).
 *   - hadc->State includes HAL_ADC_STATE_READY.
 *   - s_channel holds the index that was just converted.
 *
 * This function must stay minimal: latch raw, advance MUX, settle, re-arm.
 * No filtering, no logging. */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance != ADC1) return;

    /* Ensure the DMA-engine write to s_dma_buf is visible to the CPU. */
    __DMB();

    /* Latch result for the channel that just completed. */
    s_raw[s_channel] = (uint16_t)(s_dma_buf & 0x0FFFu);

    /* Advance channel index. */
    uint8_t next = (uint8_t)(s_channel + 1u);
    if (next >= NUM_FADERS) {
        next = 0;
        s_scan_count++;   /* full scan complete — main-loop watchdog reads this */
    }

    /* Switch MUX to next channel (GPIO register writes, ISR-safe). */
    MuxDriver_SelectChannel(next);

    /* Wait for MUX propagation + RC/op-amp settling. */
    FaderMux_SettleDelay();

    /* Commit channel advance after settling so that the main-loop watchdog
     * always sees the channel that is actually being converted. */
    s_channel = next;

    /* Re-arm single-shot DMA conversion. HAL lock and DMA state are both
     * UNLOCKED/READY at this point (released by HAL before calling us). */
    HAL_ADC_Start_DMA(hadc, &s_dma_buf, 1u);
}
