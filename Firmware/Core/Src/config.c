#include "config.h"


void Set_Config_Default(Config_t *device_config)
{
    device_config->brightness = BRIGHTNESS_DEFAULT;
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        device_config->current_activate_faders[i] = FADER_MODE_DEFAULT;
        device_config->cv_range_min[i] = CV_MIN_VALUE;
        device_config->cv_range_max[i] = CV_MAX_VALUE;
        device_config->midi_range_max[i] = MIDI_MAX_VALUE;
        device_config->midi_range_min[i] = MIDI_MIN_VALUE;
        device_config->midi_channels[i] = MIDI_CH_DEFAULT;
        device_config->midi_ccs[i] = MIDI_CCS_DEFAULT;
    }
    
}