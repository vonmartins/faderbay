#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin, uint8_t state);

uint8_t GPIO_Get(GPIO_TypeDef *port, uint16_t pin);

#endif /* GPIO_H */
