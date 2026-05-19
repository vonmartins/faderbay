#ifndef MIDI_DRIVER_H
#define MIDI_DRIVER_H

#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>


fb_err_t MidiDriver_Init(UART_HandleTypeDef *huart);

fb_err_t MidiDriver_SendCC(uint8_t channel, uint8_t cc, uint8_t value);

#endif /* MIDI_DRIVER_H */
