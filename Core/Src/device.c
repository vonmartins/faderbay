/* Device APIs (Init device, manage other APIs) Maybe do it on main.c? */

#include "device.h"
#include "flash.h"
#include <math.h>

extern Device_t device;


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
        Write_Configs(&device.current_config);
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


void Erase_Preset(Device_t *device, uint8_t preset_index)
{
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        device->presets[preset_index-1].preset_ccs[i] = MIDI_CCS_DEFAULT;
        device->presets[preset_index-1].preset_channels[i] = MIDI_CH_DEFAULT;
    }
}