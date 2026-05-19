#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "faderbay_types.h"

fb_err_t ADC_StartConversion(ADC_HandleTypeDef *hadc);
fb_err_t ADC_GetValue(ADC_HandleTypeDef *hadc, uint32_t *value);

#endif /* ADC_H */
