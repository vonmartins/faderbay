#ifndef MUX_H
#define MUX_H

#include <stdint.h>
//#include "stm32f4xx_hal_gpio.h"
#include "main.h"


/* FNS PROTOTYPES */

/* Return witch mux */
uint8_t Get_Mux(uint8_t fader); 

/* Return witch select number. From 0 to 7 */
uint8_t Get_Select(uint8_t fader);


#endif