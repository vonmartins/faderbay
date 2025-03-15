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
        if (Fader_Poll(&faders[i]) != DEVICE_OK) {
            return -1; 
        }
        Smooth_Fader(&faders[i]);
        if (faders[i].last_smooth_value != faders[i].smooth_value)
        {
            faders[i].last_smooth_value = faders[i].smooth_value;
            return i; 
        }
    }
    return -1; 
}

void Encoders_Init(Encoder_t *encoder)
{
    
}





device_error_t UI_Init(UI_Handle_t *UI)
{
    if (UI == NULL) {
        return DEVICE_INVALID_PARAM;  // UI no puede ser NULL
    }

    // Inicializar botones
    Buttons_Init(UI->buttons);

    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        if (UI->buttons[i].port == NULL || UI->buttons[i].pin == 0) {
            return DEVICE_UI_INIT_ERROR;  // Error si un botón no tiene puerto o pin asignado
        }
    }

    // Inicializar faders
    Faders_Init(UI->faders);

    for (uint8_t i = 0; i < NUM_FADERS; i++) {
        if (UI->faders[i].hadc == NULL) {
            return DEVICE_UI_INIT_ERROR;  // Error si un fader no tiene ADC asignado
        }
    }

    //Encoders_Init(&UI->encoder); 

    return DEVICE_OK;  
}



void UI_Poll(UI_Handle_t *UI)
{
    // Poll Buttons
    int8_t button_pressed = Buttons_Update(UI->buttons);
    if (button_pressed != -1) {
        UI->active_button = (uint8_t)button_pressed;
        Process_Button(UI->buttons);
    }

    // Poll Faders
    int8_t fader_changed = Faders_Update(UI->faders);
    if (fader_changed != -1) {
        UI->active_fader = (uint8_t)fader_changed;
        Process_Fader(UI->faders);
    }

    // Poll Encoder
    /* TO DO */
}
