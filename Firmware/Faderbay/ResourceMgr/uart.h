#ifndef UART_H
#define UART_H

// ============================= INCLUDES ==============================

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "faderbay_types.h"

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

/* Blocking send — used by NanoLog on UART2 */
fb_err_t UART_SendByte(UART_HandleTypeDef *huart, uint8_t byte);
fb_err_t UART_SendBuffer(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len);

/* Non-blocking DMA TX — used by MIDI driver on UART5 */
fb_err_t UART_TxInit_DMA(UART_HandleTypeDef *huart);
fb_err_t UART_TxSend(const uint8_t *bytes, uint16_t len);
void     UART_TxDmaCompleteCallback(UART_HandleTypeDef *huart);

#endif /* UART_H */
