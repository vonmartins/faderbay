#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "faderbay_types.h"

fb_err_t UART_SendByte(UART_HandleTypeDef *huart, uint8_t byte);
fb_err_t UART_SendBuffer(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len);

#endif /* UART_H */
