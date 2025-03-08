/* Fns prototypes, struct, defines */

#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* DEFINES */

#define BUTTON_RELEASED     0
#define BUTTON_PRESSED      1

#define DEBOUNCE_THRESHOLD 5


/* STRUCTS */

typedef struct {
    uint8_t name;            
    GPIO_TypeDef *port;       
    uint16_t pin;            
    uint8_t isPressed;        // Estado actual (1 = presionado, 0 = suelto)
    uint8_t lastState;        
    uint8_t debounceCounter; 
    uint8_t pressedFlag;      // Se activa en un flanco de bajada
} Button_t;



/* API PROTOTYPES */

uint8_t Button_Get_State(Button_t *button);




#endif