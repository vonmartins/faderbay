#ifndef DEVICE_H
#define DEVICE_H


#include "config.h"
#include "UI.h"






/* DEFINES */

 #define NUM_FADERS     16
 #define NUM_BUTTONS     4

 #define NUM_PRESETS    16

 #define UI_MODE_PERFORMANCE     0
 #define UI_MODE_EDITING         1
 #define UI_MODE_CONFIG          2

 #define EDIT_MODE_CH           0
 #define EDIT_MODE_CCS          1

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
    Preset_t presets[NUM_PRESETS];
    Config_t current_config;
    uint8_t current_preset;
    uint8_t selected_fader;
    uint8_t selected_config;

    /* Flags */
    uint8_t ui_mode; /* Performance, editing */
    uint8_t edit_mode;

} Device_t;




/* PROTOTYPES */

void Process_Fader(Fader_t *faders);
void Process_Button(Button_t *buttons);
void Erase_Preset(Device_t *device, uint8_t preset_index);

#endif