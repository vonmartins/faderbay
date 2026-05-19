#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

// ============================= INCLUDES ==============================

#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t EncoderDriver_Init(TIM_HandleTypeDef *htim);

void EncoderDriver_Process(void);

int16_t EncoderDriver_GetDelta(void);

#endif /* ENCODER_DRIVER_H */
