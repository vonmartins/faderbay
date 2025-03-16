/* Device APIs (Init device, manage other APIs) Maybe do it on main.c? */

#include "device.h"
#include "flash.h"
#include <math.h>
#include "MIDI/midi.h"
#include <stdio.h>
#include "device_def.h"
#include "main.h"

extern Device_t device;


const char* Device_GetErrorMessage(device_error_t error) {
    switch (error) {
        case DEVICE_OK: return "Operación exitosa";
        case DEVICE_ERROR: return "Error desconocido";
        case DEVICE_INVALID_PARAM: return "Parámetro inválido";
        case DEVICE_UART_ERROR: return "Fallo en UART";
        case DEVICE_NOT_READY: return "Dispositivo no está listo";
        default: return "Código de error desconocido";
    }
}


/* API */

uint8_t mapValue(uint8_t input, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) {
    float aux = (float)(input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return (uint8_t)ceil(aux);
}

/* Value goes from 0-127 */
void Set_Midi_Channel(uint8_t index, uint8_t value)
{
    device.current_config.midi_channels[index] = mapValue(value, 0, 127, 1, 16);
}

void Set_Midi_CC(uint8_t index, uint8_t value)
{
    device.current_config.midi_channels[index] = value;
}

void Process_Fader(Fader_t *faders)
{
    /* TO DO */
    /* Validate device flags (current menu, is editing, etc) */
    uint8_t ui_mode = device.ui_mode;
    uint8_t edit_mode = device.edit_mode;
    uint8_t fader = device.UI.active_fader;
    uint8_t fader_value = faders[fader].smooth_value;

    if (ui_mode == UI_MODE_PERFORMANCE)
    {
        /* TO DO: Set MIDI value */
        PRINT("Midi To Send\r\n");
        device.midi_to_send = 1;

    } else if (ui_mode == UI_MODE_EDITING)
    {

    } else if (ui_mode == UI_MODE_CONFIG)
    {
        
    }
}


void Process_Button(Button_t *buttons)
{
    /* TO DO */
    /* Validate device flags (current menu, is editing, etc) */
    uint8_t ui_mode = device.ui_mode;
    uint8_t edit_mode = device.edit_mode;
    uint8_t button = buttons->name;

    /* Gestión de modos de UI (Performance, Editing, Config) */
    if (button == BUTTON_MODE) {
        // Cambiar entre modos: Performance, Editing, Config (modo cíclico)
        device.ui_mode = (device.ui_mode + 1) % 3; // Esto asegura que solo haya 3 modos y que se repita
    } else if (button == BUTTON_CONF)
    {
        DEVICE_PROCESS(Write_Configs(&device.current_config));
    }

    // Modo Performance
    if (ui_mode == UI_MODE_PERFORMANCE) {
        // Aquí puedes agregar más lógicas si es necesario para el modo Performance

    } else if (ui_mode == UI_MODE_EDITING)
    {
        if (button == BUTTON_UP) {
            /* Se cambia entre modo Channel o CCs */
            if (device.edit_mode == 1) {
                device.edit_mode = 0;
            } else device.edit_mode++; /* Solo hay 2 modos */
            
        } else if (button == BUTTON_DOWN) {
            if (device.edit_mode == 0) {
                device.edit_mode = 1;
            } else device.edit_mode--;
        }

    } else if (ui_mode == UI_MODE_CONFIG)
    {
        if (button == BUTTON_UP) {
            /* Se cambia entre configuraciones */
            if (device.selected_config == NUM_CONFIGS) {
                device.selected_config = 0;
            } else device.selected_config++; 
        } else if (button == BUTTON_DOWN) {
            if (device.selected_config == 0) {
                device.selected_config = NUM_CONFIGS;
            } else device.selected_config--;
        }
    }
}

void Process_Encoder(Encoder_t *encoder)
{
        /* TO DO */
    /* Validate device flags (current menu, is editing, etc) */
    uint8_t ui_mode = device.ui_mode;
    uint8_t edit_mode = device.edit_mode;
    uint8_t selected_fader = device.selected_fader;
    uint8_t encoder_value = encoder->position;

    if (ui_mode == UI_MODE_PERFORMANCE)
    {
        
    } else if (ui_mode == UI_MODE_EDITING)
    {
        if (edit_mode == EDIT_MODE_CH)
        {
            Set_Midi_Channel(selected_fader, encoder_value);
        } 
        else if (edit_mode == EDIT_MODE_CCS)
        {
            Set_Midi_CC(selected_fader, encoder_value);
        }
    } 
    else if (ui_mode == UI_MODE_CONFIG)
    {

    }
}

/* Init presets only for first init */
void Init_Presets()
{
    for (uint8_t i = 0; i < NUM_PRESETS; i++)
    {
        device.presets[i].preset_num = (i + 1);
        for (uint8_t k = 0; k < NUM_FADERS; k++){
            device.presets[i].preset_channels[k] = MIDI_CH_DEFAULT;
            device.presets[i].preset_ccs[k] = MIDI_CH_DEFAULT;
        }
    } 
}


void Erase_Preset(Device_t *device, uint8_t preset_index)
{
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        device->presets[preset_index-1].preset_ccs[i] = MIDI_CCS_DEFAULT;
        device->presets[preset_index-1].preset_channels[i] = MIDI_CH_DEFAULT;
    }
}


device_error_t Midi_Update()
{
    if (device.midi_to_send)
    {
        midi_msg msg = {
            .header = 0xb0 | (device.current_config.midi_channels[device.selected_fader] - 1),
            .data1 = device.current_config.midi_ccs[device.selected_fader],
            .data2 = device.UI.faders[device.selected_fader].smooth_value
        };

        if(sendMidiCC(device.midi_handle, &msg))
        {
            device.midi_to_send = 0; /* Mover fuera el midi_to_send para que se active aunq haya error de uart? */
            return DEVICE_OK;

        } else {
            return DEVICE_UART_ERROR;
        } 
    }
}