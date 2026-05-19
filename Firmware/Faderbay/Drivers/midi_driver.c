/* =========================================================
 * midi_driver.c
 * Driver layer — Low-level MIDI UART framing driver
 * ========================================================= */

#include "midi_driver.h"
#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include "uart.h"


static UART_HandleTypeDef * s_huart;

/* PRIVATE FNS */

static fb_err_t send_message(uint8_t *msg, uint8_t len) {
    if (msg == NULL || len == 0) return FB_ERR_INVALID_PARAM;
    return UART_SendBuffer(s_huart, msg, len);
}

/* PUBLIC FNS */

fb_err_t MidiDriver_Init(UART_HandleTypeDef *huart) {
    if (huart == NULL) return FB_ERR_INVALID_PARAM;
    s_huart = huart;
    return FB_OK;
}

fb_err_t MidiDriver_SendCC(uint8_t channel, uint8_t cc, uint8_t value) {
    if (channel < 1 || channel > 16) return FB_ERR_INVALID_PARAM;
    if (cc > 127 || value > 127)     return FB_ERR_INVALID_PARAM;

    uint8_t msg[3];
    msg[0] = 0xB0 | (channel - 1);      // status byte: CC + canal
    msg[1] = cc;                        // número de CC
    msg[2] = value;                     // valor
    return send_message(msg, 3);
}
