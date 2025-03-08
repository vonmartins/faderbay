#include <stdio.h>
#include "UI.h"
#include "main.h"
#include "stm32f411xe.h"



/* VARIABLES */

const struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} Button_Config[NUM_BUTTONS] = {
    {BTTN1_GPIO_Port, BTTN1_Pin},
    {BTTN2_GPIO_Port, BTTN2_Pin},
    {BTTN3_GPIO_Port, BTTN3_Pin},
    {BTTN4_GPIO_Port, BTTN4_Pin}
};

extern ADC_HandleTypeDef hadc1;


/* API */

void Buttons_Init(Button_t *buttons) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].name = i;
        buttons[i].port = Button_Config[i].port;
        buttons[i].pin = Button_Config[i].pin;
        buttons[i].isPressed = 0;
        buttons[i].lastState = 1;   // Asume que el botón empieza sin presionar
        buttons[i].debounceCounter = 0;
        buttons[i].pressedFlag = 0;
    }
}

/* Return button changed or -1 if no button changed */
int8_t Buttons_Update(Button_t *buttons) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        if (Button_Get_State(&buttons[i])) {
            buttons[i].pressedFlag = 0; 
            return i; 
        }
    }
    return -1; 
}


/* Faders Init */
void Faders_Init(Fader_t *faders)
{
    
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        faders[i].name = i;
        faders[i].smooth_value = FADER_DEFAULT; /* Init to 0 */
        faders[i].mux = i / 8;
        faders[i].mux_channel = i % 8;
        faders[i].hadc = &hadc1;
    }
}


/* Return fader changed or -1 if no fader changed */
int8_t Faders_Update(Fader_t *faders)
{
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        uint32_t raw = Faders_Get_Raw_Value(faders, i);
        if (faders[i].raw_value != raw)
        {
            faders[i].raw_value = raw;
            Smooth_Fader(&faders[i]);
            return i; 
        }
    }
    return -1;  // -1 si ningún fader cambió
}

void Encoders_Init(Encoder_t *encoder)
{
    
}





void UI_Init(Device_t *device)
{
    Buttons_Init(device->buttons);
    Faders_Init(device->faders);
    //Encoders_Init(&device->encoder);

}


void UI_Poll(Device_t *device) {
    // Poll Buttons
    int8_t button_pressed = Buttons_Update(device->buttons);
    if (button_pressed != -1) {
        device->active_button = (uint8_t)button_pressed;
        Process_Button(device);
    }

    // Poll Faders
    int8_t fader_changed = Faders_Update(device->faders);
    if (fader_changed != -1) {
        device->active_fader = (uint8_t)fader_changed;
        Process_Fader(device);
    }

    // Poll Encoder
    /* TO DO*/

}
