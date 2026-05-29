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

/* Filter task — runs in scheduler, applies IIR and watchdog. No ADC polling. */
void FaderDriver_Process(void);

/* Raw ADC value for the last completed conversion on channel index. */
uint16_t FaderDriver_GetRaw(uint8_t index);

/* IIR-filtered value, updated by FaderDriver_Process. Use this for MIDI. */
uint16_t FaderDriver_GetFiltered(uint8_t index);

#endif /* FADER_DRIVER_H */
