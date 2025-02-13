/* Fns prototypes, struct, defines */

#ifndef FADER_H
#define FADER_H

#include <stdint.h>

/* DEFINES */

#define MAX_MAPPED_VALUE    127
#define MIN_MAPPED_VALUE    0
#define MAX_RAW_VALUE       4095
#define MIN_RAW_VALUE       0

#define FADERS_ADC_HANDLE   


/* STRUCTS */

typedef struct {
    uint32_t last_value;    
    uint32_t raw_value; /* 0 - 4096 (12 bits ADC) */
    uint8_t smooth_value; /* 0 - 127 (MIDI) */
} Fader_t;


/* API PROTOTYPES */

void Faders_Init(Fader_t *faders);
void Smooth_Fader(Fader_t *fader);
int8_t Faders_Update(Fader_t *faders); /* Return fader updated */
uint16_t Faders_Get_Raw_Value(Fader_t *faders, uint8_t fader);
uint8_t Faders_Get_Smooth_Value(Fader_t *faders, uint8_t fader);

#endif