#ifndef UI_H
#define UI_H

#include "fader.h"
#include "button.h"
#include "encoder.h"
#include "app_config.h"
#include "device_def.h"

/* DEFINES */

#define BUTTON_MODE     0
#define BUTTON_CONF     1
#define BUTTON_UP       2
#define BUTTON_DOWN     3

#define NUM_BUTTONS     4


typedef struct {
    Fader_t faders[NUM_FADERS];
    Button_t buttons[NUM_BUTTONS];
    Encoder_t encoder;
    uint8_t active_fader;
    uint8_t active_button;
} UI_Handle_t;


/* FNS PROTOTYPES */

void Buttons_Init(Button_t *buttons);
int8_t Buttons_Update(Button_t *buttons);

void Faders_Init(Fader_t *faders);
int8_t Faders_Update(Fader_t *faders);


void Encoders_Init(Encoder_t *encoder);


device_error_t UI_Init(UI_Handle_t *UI);
void UI_Poll(UI_Handle_t *UI);


#endif