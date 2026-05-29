/* =========================================================
 * encoder_driver.c
 * Driver layer — Quadrature encoder with detent normalisation and acceleration
 *
 * ARCHITECTURE
 * ============
 * TIM3 runs in hardware quadrature encoder mode (TI1+TI2, 4× counting).
 * Process() polls the counter every PERIOD_ENCODER_DRIVER ms and adds the
 * signed difference to s_raw_accum. Each time |s_raw_accum| reaches
 * ENCODER_COUNTS_PER_DETENT, a logical detent is extracted, multiplied by the
 * velocity factor, and added to s_logical_delta.
 *
 * JITTER SUPPRESSION
 * ==================
 * Mechanical encoders produce ±1 count glitches at rest. Because a glitch
 * of ±(ENCODER_COUNTS_PER_DETENT - 1) is required to emit even one logical
 * detent, the raw accumulator absorbs all sub-detent jitter without emitting
 * false deltas. Opposite-sign glitches cancel inside the accumulator.
 *
 * ACCELERATION
 * ============
 * Velocity is measured as Δt between consecutive logical-detent batches.
 * The first detent after idle always uses multiplier 1 (no prior reference).
 *
 *   Δt < ENCODER_ACCEL_FAST_MS (20 ms)  → ×ENCODER_ACCEL_FAST_MULT (4)
 *   Δt < ENCODER_ACCEL_MED_MS  (50 ms)  → ×ENCODER_ACCEL_MED_MULT  (2)
 *   Δt ≥ ENCODER_ACCEL_MED_MS           → ×1  (no acceleration)
 *
 * Disabled globally by setting ENCODER_ACCEL_ENABLE to 0 in config.h.
 *
 * WRAPAROUND LIMIT
 * ================
 * The TIM counter is 16-bit. The wrap-safe cast (int16_t)(current - last)
 * is valid as long as the encoder does not move more than 32767 counts
 * (≈ 8191 detents) between two consecutive Process() calls. At 5 ms per
 * call and any realistic mechanical encoder speed this is never approached.
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "encoder_driver.h"
#include "config.h"
#include "nlog.h"
#include "scheduler.h"
#include <stdint.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "ENC_DRV";

static TIM_HandleTypeDef *s_htim;
static uint16_t  s_last_count;

/* Partial raw-count carry between Process() calls.
 * Invariant: |s_raw_accum| < ENCODER_COUNTS_PER_DETENT after each Process(). */
static int32_t   s_raw_accum;

/* Logical detent accumulator consumed by GetDelta(). */
static int16_t   s_logical_delta;

/* Raw-count accumulator consumed by GetRawDelta() (debug). */
static int16_t   s_raw_delta;

/* Acceleration state. */
static uint32_t  s_last_detent_tick;  /* tick of last detent batch emission */
static uint8_t   s_accel_valid;       /* 0 until first detent → forces mult=1 */

// ========================= PRIVATE FUNC. DECL. =======================

static int16_t accel_mult(uint32_t dt_ms);

// =========================== PRIVATE FUNCTIONS =======================

static int16_t accel_mult(uint32_t dt_ms)
{
#if ENCODER_ACCEL_ENABLE
    if (dt_ms < (uint32_t)ENCODER_ACCEL_FAST_MS) return (int16_t)ENCODER_ACCEL_FAST_MULT;
    if (dt_ms < (uint32_t)ENCODER_ACCEL_MED_MS)  return (int16_t)ENCODER_ACCEL_MED_MULT;
#else
    (void)dt_ms;
#endif
    return 1;
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t EncoderDriver_Init(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        LOGE(TAG, "Init: null TIM handle");
        return FB_ERR_INVALID_PARAM;
    }
    s_htim             = htim;
    s_raw_accum        = 0;
    s_logical_delta    = 0;
    s_raw_delta        = 0;
    s_last_detent_tick = 0;
    s_accel_valid      = 0;

    if (HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL) != HAL_OK) {
        LOGE(TAG, "Init: HAL_TIM_Encoder_Start failed");
        return FB_ERR_TIMER;
    }
    s_last_count = (uint16_t)__HAL_TIM_GET_COUNTER(htim);
    LOGI(TAG, "Init OK");
    return FB_OK;
}

void EncoderDriver_Process(void)
{
    /* --- 1. Compute raw TIM delta (wrap-safe for 16-bit counter) ------- */
    uint16_t current  = (uint16_t)__HAL_TIM_GET_COUNTER(s_htim);
    int16_t  raw_diff = (int16_t)(current - s_last_count);
    s_last_count = current;

    s_raw_delta += raw_diff;   /* debug accumulator, consumed by GetRawDelta() */

    if (raw_diff == 0) return;

    /* --- 2. Accumulate into raw carry ---------------------------------- */
    s_raw_accum += raw_diff;

    /* --- 3. Extract whole logical detents ------------------------------ */
    /* Integer division truncates toward zero, preserving sign.
     * Remainder (partial detent) is kept in s_raw_accum for next call. */
    int16_t detents = (int16_t)(s_raw_accum / (int32_t)ENCODER_COUNTS_PER_DETENT);
    if (detents == 0) return;   /* sub-detent motion: jitter absorbed, nothing to emit */

    s_raw_accum -= (int32_t)detents * (int32_t)ENCODER_COUNTS_PER_DETENT;

    /* --- 4. Apply velocity acceleration -------------------------------- */
    uint32_t now  = Scheduler_GetTick();
    int16_t  mult = 1;

    if (s_accel_valid) {
        uint32_t dt = now - s_last_detent_tick;
        mult = accel_mult(dt);
    } else {
        s_accel_valid = 1;   /* first detent: no prior reference, skip acceleration */
    }
    s_last_detent_tick = now;

    s_logical_delta += detents * mult;
}

int16_t EncoderDriver_GetDelta(void)
{
    int16_t d       = s_logical_delta;
    s_logical_delta = 0;
    return d;
}

int16_t EncoderDriver_GetRawDelta(void)
{
    int16_t d   = s_raw_delta;
    s_raw_delta = 0;
    return d;
}
