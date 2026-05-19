/* =========================================================
 * encoder_driver.c
 * Driver layer — Quadrature encoder input driver
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "encoder_driver.h"
#include <stdint.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static TIM_HandleTypeDef * s_htim;
static uint16_t s_last_count;
static int16_t s_delta;

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t EncoderDriver_Init(TIM_HandleTypeDef *htim) {
    if (htim == NULL) return FB_ERR_INVALID_PARAM;
    s_htim = htim;
    s_last_count = 0;
    s_delta = 0;
    if (HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL) != HAL_OK) {
        return FB_ERR_TIMER;
    }
    return FB_OK;
}

void EncoderDriver_Process(void) {
    uint16_t current = __HAL_TIM_GET_COUNTER(s_htim);
    s_delta += (int16_t)(current - s_last_count);
    s_last_count = current;
}

int16_t EncoderDriver_GetDelta(void) {
    int16_t delta = s_delta;
    s_delta = 0;
    return delta;
}
