/* =========================================================
 * uart.c
 * Resource Manager layer — UART peripheral wrapper
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "uart.h"
#include "nlog.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "UART";

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t UART_SendBuffer(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len)
{
    if (huart == NULL || buf == NULL || len == 0) {
        LOGE(TAG, "SendBuffer: invalid param");
        return FB_ERR_INVALID_PARAM;
    }
    if (HAL_UART_Transmit(huart, buf, len, 10) != HAL_OK) {
        LOGE(TAG, "SendBuffer: HAL transmit failed");
        return FB_ERR_UART;
    }
    return FB_OK;
}

fb_err_t UART_SendByte(UART_HandleTypeDef *huart, uint8_t byte)
{
    return UART_SendBuffer(huart, &byte, 1);
}
