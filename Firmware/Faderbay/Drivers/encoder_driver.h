#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

fb_err_t EncoderDriver_Init(TIM_HandleTypeDef *htim);

void EncoderDriver_Process(void);

int16_t EncoderDriver_GetDelta(void);

#endif /* ENCODER_DRIVER_H */
