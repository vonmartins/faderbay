/* =========================================================
 * gpio.c
 * Resource Manager layer — GPIO pin abstraction
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "gpio.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin, uint8_t state) {
    if (port == NULL) return;
    HAL_GPIO_WritePin(port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t GPIO_Get(GPIO_TypeDef *port, uint16_t pin) {
    if (port == NULL) return 0;
    return HAL_GPIO_ReadPin(port, pin);
}
