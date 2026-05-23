#ifndef FADER_CONTROL_H
#define FADER_CONTROL_H

#include "faderbay_types.h"
#include <stdint.h>

fb_err_t FaderControl_Init(void);

void FaderControl_Process(void);

uint8_t FaderControl_GetMidi(uint8_t index);

uint8_t FaderControl_HasChanged(uint8_t index);

#endif /* FADER_CONTROL_H */
