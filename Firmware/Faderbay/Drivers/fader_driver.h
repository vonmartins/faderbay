#ifndef FADER_DRIVER_H
#define FADER_DRIVER_H

// ============================= INCLUDES ==============================

#include "faderbay_types.h"
#include "adc.h"
#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t FaderDriver_Init(ADC_HandleTypeDef *hadc);

void FaderDriver_Process(void);

uint16_t FaderDriver_GetRaw(uint8_t index);

#endif /* FADER_DRIVER_H */
