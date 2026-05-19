/* =========================================================
 * adc.c
 * Resource Manager layer — ADC peripheral wrapper and channel mux
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "adc.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t ADC_StartConversion(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL) return FB_ERR_INVALID_PARAM;
    if (HAL_ADC_Start(hadc) != HAL_OK) return FB_ERR_ADC;
    return FB_OK;
}

fb_err_t ADC_GetValue(ADC_HandleTypeDef *hadc, uint32_t *value)
{
    if (hadc == NULL || value == NULL) return FB_ERR_INVALID_PARAM;
    if (HAL_ADC_PollForConversion(hadc, 10) != HAL_OK) return FB_ERR_ADC;
    *value = HAL_ADC_GetValue(hadc);
    return FB_OK;
}
