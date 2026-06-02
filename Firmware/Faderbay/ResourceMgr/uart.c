/* =========================================================
 * uart.c
 * Resource Manager layer — UART peripheral wrapper
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "uart.h"
#include "config.h"
#include "nlog.h"
#include <stdbool.h>

// =========================== PRIVATE DEFINES =========================

#define RING_MASK  ((uint16_t)(UART_TX_RING_SIZE - 1u))

// ============================ PRIVATE TYPES ==========================

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t             ring_buf[UART_TX_RING_SIZE];
    volatile uint16_t   head;        /* written by producer (main loop) */
    volatile uint16_t   tail;        /* written by DMA ISR */
    volatile uint16_t   in_flight;   /* bytes owned by current DMA transfer */
    volatile bool       tx_busy;
    volatile uint32_t   full_event_count;
} UartTxCtx_t;

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "UART";

static UartTxCtx_t s_uart5_tx_ctx;

// ========================= PRIVATE FUNC. DECL. =======================

static uint16_t ring_used(UartTxCtx_t *c);
static uint16_t ring_free(UartTxCtx_t *c);
static uint16_t ring_contiguous_from_tail(UartTxCtx_t *c);
static void     start_dma(UartTxCtx_t *c);

// =========================== PRIVATE FUNCTIONS =======================

static uint16_t ring_used(UartTxCtx_t *c)
{
    return (uint16_t)((c->head + UART_TX_RING_SIZE - c->tail) & RING_MASK);
}

static uint16_t ring_free(UartTxCtx_t *c)
{
    return (uint16_t)(UART_TX_RING_SIZE - 1u - ring_used(c));
}

/* Contiguous bytes available from tail before the buffer wraps. */
static uint16_t ring_contiguous_from_tail(UartTxCtx_t *c)
{
    uint16_t used   = ring_used(c);
    uint16_t to_end = (uint16_t)(UART_TX_RING_SIZE - c->tail);
    return (used < to_end) ? used : to_end;
}

/* Arm a new DMA transfer starting at tail. Must be called with tx_busy == true. */
static void start_dma(UartTxCtx_t *c)
{
    c->in_flight = ring_contiguous_from_tail(c);
    if (HAL_UART_Transmit_DMA(c->huart, c->ring_buf + c->tail, c->in_flight) != HAL_OK) {
        c->in_flight = 0;
        c->tx_busy   = false;
    }
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t UART_TxInit_DMA(UART_HandleTypeDef *huart)
{
    if (huart == NULL) return FB_ERR_INVALID_PARAM;
    UartTxCtx_t *c  = &s_uart5_tx_ctx;
    c->huart         = huart;
    c->head          = 0;
    c->tail          = 0;
    c->in_flight     = 0;
    c->tx_busy       = false;
    c->full_event_count = 0;
    return FB_OK;
}

fb_err_t UART_TxSend(const uint8_t *bytes, uint16_t len)
{
    if (bytes == NULL || len == 0) return FB_ERR_INVALID_PARAM;

    UartTxCtx_t *c = &s_uart5_tx_ctx;

    if (ring_free(c) < len) {
        if ((c->full_event_count++ % 1000u) == 0u) {
            LOGW(TAG, "ring full");
        }
        return FB_ERR_NOT_READY;
    }

    /* Copy bytes into ring, handling wrap with byte-by-byte loop. */
    for (uint16_t i = 0; i < len; i++) {
        c->ring_buf[(c->head + i) & RING_MASK] = bytes[i];
    }
    c->head = (c->head + len) & RING_MASK;

    /* Critical section: arm DMA if idle. Prevents TOCTOU with the ISR
     * that could clear tx_busy between the check and the arm. */
    __disable_irq();
    if (!c->tx_busy) {
        c->tx_busy = true;
        start_dma(c);
    }
    __enable_irq();

    return FB_OK;
}

/* Called from HAL_UART_TxCpltCallback (ISR context). No logs, no blocking. */
void UART_TxDmaCompleteCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != UART5) return;
    UartTxCtx_t *c = &s_uart5_tx_ctx;

    c->tail      = (c->tail + c->in_flight) & RING_MASK;
    c->in_flight = 0;

    if (ring_used(c) > 0u) {
        start_dma(c);   /* drain next contiguous chunk */
    } else {
        c->tx_busy = false;
    }
}

/* Override the HAL weak callback to hook into our ring. */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UART_TxDmaCompleteCallback(huart);
}

// ====================== LEGACY BLOCKING API ==========================

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
