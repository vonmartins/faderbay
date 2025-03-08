#ifndef DEVICE_H
#define DEVICE_H


#include "config.h"
#include "UI.h"


/* DEFINES */

 #define NUM_FADERS     16
 #define NUM_BUTTONS     4

 #define NUM_PRESETS    16

/* STRUCTS */

/* Struct to save preset configs */
typedef struct {
    uint8_t preset_num;
    uint8_t preset_channels[NUM_FADERS];
    uint8_t preset_ccs[NUM_FADERS];
} Preset_t;

typedef struct {
    /* UI */
    UI_Handle_t UI;

    /* Configs */
    Preset_t current_presets[NUM_PRESETS];
    Config_t current_config;


} Device_t;




/* PROTOTYPES */

void Process_Fader(Fader_t *fader);
void Process_Button(Button_t *button);

#endif