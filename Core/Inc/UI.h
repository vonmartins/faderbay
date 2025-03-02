#ifndef UI_H
#define UI_H

#include "device.h"
#include "fader.h"
#include "button.h"
#include "encoder.h"
#include "app_config.h"

/* DEFINES */

#define BUTTON_1    0
#define BUTTON_2    1
#define BUTTON_3    2
#define BUTTON_4    3

#define NUM_BUTTONS     4





/* FNS PROTOTYPES */

void Buttons_Init(Button_t *buttons);
int8_t Buttons_Update(Button_t *buttons);

void Faders_Init(Fader_t *faders);
int8_t Faders_Update(Fader_t *faders); 


void Encoders_Init(Encoder_t *encoder);


void UI_Init(Device_t *device);
void UI_Poll(Device_t *device);


#endif