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

static UART_HandleTypeDef * s_huart;

// ========================= PRIVATE FUNC. DECL. =======================

static fb_err_t send_message(uint8_t *msg, uint8_t len);

// =========================== PRIVATE FUNCTIONS =======================

static fb_err_t send_message(uint8_t *msg, uint8_t len) {
    if (msg == NULL || len == 0) return FB_ERR_INVALID_PARAM;
    return UART_SendBuffer(s_huart, msg, len);
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t MidiDriver_Init(UART_HandleTypeDef *huart) {
    if (huart == NULL) {
        LOGE(TAG, "Init failed: null UART handle");
        return FB_ERR_INVALID_PARAM;
    }
    s_huart = huart;
    LOGI(TAG, "Init OK");
    return FB_OK;
}

fb_err_t MidiDriver_SendCC(uint8_t channel, uint8_t cc, uint8_t value) {
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

    fb_err_t err = send_message(msg, 3);
    if (err != FB_OK) {
        LOGE(TAG, "SendCC: UART send failed");
    }
    return err;
}
