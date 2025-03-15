#ifndef _MIDI_H
#define MIDI_H

 #include <stdint.h>

 #include "stm32f4xx_hal.h"

typedef struct 
{
    uint8_t header;
    uint8_t data1;
    uint8_t data2;
} midi_msg;


uint8_t sendMidiCC(UART_HandleTypeDef *huart, midi_msg *msg);





#endif