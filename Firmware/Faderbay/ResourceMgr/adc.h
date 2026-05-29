#ifndef ADC_H
#define ADC_H

// ============================= INCLUDES ==============================

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "faderbay_types.h"

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t ADC_StartConversion(ADC_HandleTypeDef *hadc);
fb_err_t ADC_GetValue(ADC_HandleTypeDef *hadc, uint32_t *value);
fb_err_t ADC_StartConversionDMA(ADC_HandleTypeDef *hadc, uint32_t *buffer, uint32_t length);

#endif /* ADC_H */
