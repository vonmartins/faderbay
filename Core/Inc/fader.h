/* Fns prototypes, struct, defines */

#ifndef FADER_H
#define FADER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"

/* DEFINES */

#define MAX_MAPPED_VALUE    127
#define MIN_MAPPED_VALUE    0
#define MAX_RAW_VALUE       4095
#define MIN_RAW_VALUE       0

#define FADERS_ADC_HANDLE   ADC1

#define MUX1    0
#define MUX2    1



/* STRUCTS */

typedef struct {
    uint8_t name;
    // uint8_t fader_changed; /* Flag to process MIDI. Put to 0 after processing the fader */
    uint32_t raw_value; /* 0 - 4096 (12 bits ADC) */
    uint8_t smooth_value; /* 0 - 127 (MIDI) */
    ADC_HandleTypeDef *hadc;
    uint8_t mux;
    uint8_t mux_channel;
} Fader_t;


/* API PROTOTYPES */

void Smooth_Fader(Fader_t *fader);
uint16_t Faders_Get_Raw_Value(Fader_t *faders, uint8_t fader);
uint8_t Faders_Get_Smooth_Value(Fader_t *faders, uint8_t fader);
void Set_Faders_On();  /* Set LOW MUX1_INH and MUX2_INH pins */
void Set_Faders_Off();  /* Set HIGH MUX1_INH and MUX2_INH pins */




#endif