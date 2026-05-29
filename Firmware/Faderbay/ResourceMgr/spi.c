/* =========================================================
 * spi.c
 * Resource Manager layer — SPI peripheral wrapper
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "spi.h"
#include "nlog.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "SPI";

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t SPI_SendBuffer(SPI_HandleTypeDef *hspi, uint8_t *buf, uint16_t len)
{
    if (hspi == NULL || buf == NULL || len == 0) {
        LOGE(TAG, "SendBuffer: invalid param");
        return FB_ERR_INVALID_PARAM;
    }
    if (HAL_SPI_Transmit_DMA(hspi, buf, len) != HAL_OK) {
        LOGE(TAG, "DMA transmit failed");
        return FB_ERR_SPI;
    }
    // Esperar a que la transferencia termine
    while (HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY) {
        // busy wait — el DMA hace el trabajo, CPU puede atender ISRs
    }
    return FB_OK;
}

fb_err_t SPI_SendByte(SPI_HandleTypeDef *hspi, uint8_t byte)
{
    return SPI_SendBuffer(hspi, &byte, 1);
}
