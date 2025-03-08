/* Configurations (debug modes, etc) */
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>

#define FADER_MODE_OFF          0   /* Fader not activated */
#define FADER_MODE_MIDI         1   /* Fader MIDI mode only */
#define FADER_MODE_MIDI_CV      2   /* Fader MIDI and CV mode */
#define FADER_MODE_CV           3   /* Fader CV mode only*/
#define FADER_MODE_DEFAULT      FADER_MODE_MIDI_CV 

#define BRIGHTNESS_DEFAULT      50

#define MIDI_MIN_VALUE    0
#define MIDI_MAX_VALUE    127

#define CV_MIN_VALUE       0
#define CV_MAX_VALUE       4095

#define MIDI_CH_1           1
#define MIDI_CH_2           2
#define MIDI_CH_3           3
#define MIDI_CH_4           4
#define MIDI_CH_5           5
#define MIDI_CH_6           6
#define MIDI_CH_7           7
#define MIDI_CH_8           8
#define MIDI_CH_9           9
#define MIDI_CH_10          10
#define MIDI_CH_11          11
#define MIDI_CH_12          12
#define MIDI_CH_13          13
#define MIDI_CH_14          14
#define MIDI_CH_15          15
#define MIDI_CH_16          16
#define MIDI_CH_DEFAULT     MIDI_CH_1
#define MIDI_CCS_DEFAULT    1

#ifndef NUM_FADERS
#define NUM_FADERS  16
#endif


typedef struct 
{
    uint8_t brightness;
    uint8_t current_activate_faders[NUM_FADERS]; /* Which faders are activated (Midi mode) */

    uint8_t midi_range_min[NUM_FADERS];     /* Rango mínimo para cada fader */
    uint8_t midi_range_max[NUM_FADERS];     /* Rango máximo para cada fader */
    uint16_t cv_range_min[NUM_FADERS];      /* Rango mínimo para la salida CV */
    uint16_t cv_range_max[NUM_FADERS];      /* Rango máximo para la salida CV */

    uint8_t midi_channels[NUM_FADERS];      /* Midi channels for every fader */
    uint8_t midi_ccs[NUM_FADERS];           /* Midi CCs for every fader */

} Config_t;

void Set_Config_Default(Config_t *device_config);


#endif