#ifndef FADER_DRIVER_H
#define FADER_DRIVER_H

#include "faderbay_types.h"
#include "adc.h"
#include <stdint.h>

fb_err_t FaderDriver_Init(ADC_HandleTypeDef *hadc);

void FaderDriver_Process(void);

uint16_t FaderDriver_GetRaw(uint8_t index);



#endif /* FADER_DRIVER_H */
