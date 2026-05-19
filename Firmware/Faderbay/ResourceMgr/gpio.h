#ifndef GPIO_H
#define GPIO_H

// ============================= INCLUDES ==============================

#include <stdint.h>
#include "stm32f4xx_hal.h"

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin, uint8_t state);

uint8_t GPIO_Get(GPIO_TypeDef *port, uint16_t pin);

#endif /* GPIO_H */
