#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "faderbay_types.h"

fb_err_t SPI_SendByte(SPI_HandleTypeDef *hspi, uint8_t byte);
fb_err_t SPI_SendBuffer(SPI_HandleTypeDef *hspi, uint8_t *buf, uint16_t len);

#endif /* SPI_H */
