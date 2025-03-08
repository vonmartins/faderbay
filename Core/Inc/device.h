#ifndef DEVICE_H
#define DEVICE_H


#include "fader.h"
#include "encoder.h"
#include "button.h"
#include "config.h"


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
    Fader_t faders[NUM_FADERS];
    Button_t buttons[NUM_BUTTONS];
    Encoder_t encoder;
    uint8_t active_fader;
    uint8_t active_button;

    /* Configs */
    Preset_t current_presets[NUM_PRESETS];
    Config_t current_config;


} Device_t;




/* PROTOTYPES */

void Process_Fader(Device_t *device);
void Process_Button(Device_t *device);

#endif