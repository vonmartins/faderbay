/* =========================================================
 * uart.c
 * Resource Manager layer — UART peripheral wrapper
 * ========================================================= */

#include "uart.h"

fb_err_t UART_SendBuffer(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len)
{
    if (huart == NULL || buf == NULL || len == 0) return FB_ERR_INVALID_PARAM;
    if (HAL_UART_Transmit(huart, buf, len, 10) != HAL_OK) return FB_ERR_UART;
    return FB_OK;
}

fb_err_t UART_SendByte(UART_HandleTypeDef *huart, uint8_t byte)
{
    return UART_SendBuffer(huart, &byte, 1);
}
