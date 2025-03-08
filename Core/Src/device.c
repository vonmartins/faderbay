/* Device APIs (Init device, manage other APIs) Maybe do it on main.c? */

#include "device.h"



/* API */

void Process_Fader(Fader_t *fader)
{
    /* TO DO */
    /* Validate device flags (current menu, is editing, etc) */
}


void Process_Button(Button_t *button)
{
    /* TO DO */
    /* Validate device flags (current menu, is editing, etc) */
}


void Erase_Preset(Device_t *device, uint8_t preset_index)
{
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        device->presets[preset_index-1].preset_ccs[i] = MIDI_CCS_DEFAULT;
        device->presets[preset_index-1].preset_channels[i] = MIDI_CH_DEFAULT;
    }
}