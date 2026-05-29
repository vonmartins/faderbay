/* =========================================================
 * adc.c
 * Resource Manager layer — ADC peripheral wrapper and channel mux
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "adc.h"
#include "nlog.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "ADC";

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t ADC_StartConversion(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL) {
        LOGE(TAG, "StartConversion: null handle");
        return FB_ERR_INVALID_PARAM;
    }
    if (HAL_ADC_Start(hadc) != HAL_OK) {
        LOGE(TAG, "StartConversion: HAL start failed");
        return FB_ERR_ADC;
    }
    return FB_OK;
}

fb_err_t ADC_GetValue(ADC_HandleTypeDef *hadc, uint32_t *value)
{
    if (hadc == NULL || value == NULL) {
        LOGE(TAG, "GetValue: invalid param");
        return FB_ERR_INVALID_PARAM;
    }
    if (HAL_ADC_PollForConversion(hadc, 10) != HAL_OK) {
        LOGE(TAG, "GetValue: poll timeout");
        return FB_ERR_ADC;
    }
    *value = HAL_ADC_GetValue(hadc);
    return FB_OK;
}

fb_err_t ADC_StartConversionDMA(ADC_HandleTypeDef *hadc, uint32_t *buffer, uint32_t length)
{
    if (hadc == NULL || buffer == NULL || length == 0u) {
        LOGE(TAG, "StartConversionDMA: invalid param");
        return FB_ERR_INVALID_PARAM;
    }
    if (HAL_ADC_Start_DMA(hadc, buffer, length) != HAL_OK) {
        LOGE(TAG, "StartConversionDMA: HAL start failed");
        return FB_ERR_ADC;
    }
    return FB_OK;
}
