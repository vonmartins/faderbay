/* =========================================================
 * midi_driver.c
 * Driver layer — Low-level MIDI UART framing driver
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "midi_driver.h"
#include "faderbay_types.h"
#include "nlog.h"
#include "stm32f4xx_hal.h"
#include "uart.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "MIDI_DRV";

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t MidiDriver_Init(UART_HandleTypeDef *huart)
{
    if (huart == NULL) {
        LOGE(TAG, "Init failed: null UART handle");
        return FB_ERR_INVALID_PARAM;
    }
    fb_err_t err = UART_TxInit_DMA(huart);
    if (err != FB_OK) {
        LOGE(TAG, "Init failed: DMA init err %d", err);
        return err;
    }
    LOGI(TAG, "Init OK");
    return FB_OK;
}

fb_err_t MidiDriver_SendCC(uint8_t channel, uint8_t cc, uint8_t value)
{
    if (channel < 1 || channel > 16) {
        LOGE(TAG, "SendCC: invalid channel %u", channel);
        return FB_ERR_INVALID_PARAM;
    }
    if (cc > 127 || value > 127) {
        LOGE(TAG, "SendCC: invalid cc/value");
        return FB_ERR_INVALID_PARAM;
    }

    uint8_t msg[3];
    msg[0] = 0xB0 | (channel - 1);
    msg[1] = cc;
    msg[2] = value;

    fb_err_t err = UART_TxSend(msg, 3);
    if (err == FB_ERR_NOT_READY) {
        return FB_ERR_NOT_READY;
    }
    if (err != FB_OK) {
        LOGE(TAG, "SendCC: UART send failed");
        return FB_ERR_UART;
    }
    return FB_OK;
}
