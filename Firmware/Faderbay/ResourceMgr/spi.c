/* =========================================================
 * spi.c
 * Resource Manager layer — SPI peripheral wrapper
 * ========================================================= */

#include "spi.h"

fb_err_t SPI_SendBuffer(SPI_HandleTypeDef *hspi, uint8_t *buf, uint16_t len)
{
    if (hspi == NULL || buf == NULL || len == 0) return FB_ERR_INVALID_PARAM;
    if (HAL_SPI_Transmit(hspi, buf, len, 1) != HAL_OK) return FB_ERR_SPI;
    return FB_OK;
}

fb_err_t SPI_SendByte(SPI_HandleTypeDef *hspi, uint8_t byte)
{
    return SPI_SendBuffer(hspi, &byte, 1);
}
