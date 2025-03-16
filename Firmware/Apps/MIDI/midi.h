#ifndef __MIDI_H__
#define __MIDI_H__

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "midi_defs.h"




uint8_t sendMidiCC(UART_HandleTypeDef *huart, midi_msg *msg);





#endif