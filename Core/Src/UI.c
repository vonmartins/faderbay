/* UI general API (Polling, Init, etc) */

#include <stdio.h>
#include "fader.h"
#include "button.h"
#include "encoder.h"
#include "device.h"


/* VARIABLES */

/* API */

void UI_Init(Device_t *device)
{
    Faders_Init(&device->faders);
    Encoder_Init(&device->encoder);
    Button_Init();

        
}


void UI_poll()
{

}