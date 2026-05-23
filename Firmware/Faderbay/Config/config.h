#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "faderbay_types.h"

// ============================ PUBLIC DEFINES =========================

#define NUM_FADERS              16
#define NUM_BUTTONS              4
#define NUM_PRESETS             16
#define SCH_MAX_TASKS           10u

#define ADC_MAX_REAL            3890
#define FADER_CHANGE_THRESHOLD  1

#define BTN_MODE                0
#define BTN_SELECT              1
#define BTN_BACK                2
#define BTN_PARAM               3

#define MIDI_DEFAULT_CHANNEL    1
#define MIDI_BASE_CC            7

// Scheduler task periods (ms)
#define PERIOD_FADER_DRIVER       1
#define PERIOD_ENCODER_DRIVER     5
#define PERIOD_FADER_CONTROL      8
#define PERIOD_MIDI_CONTROL       8
#define PERIOD_BUTTON_DRIVER     10
#define PERIOD_UI_CONTROL        50
#define PERIOD_DISPLAY_FLUSH     50

// ============================ PUBLIC TYPES ===========================

typedef struct {
    uint8_t      midi_channel;
    uint8_t      midi_cc;
    FaderMode_t  mode;
} FaderConfig_t;

typedef struct {
    FaderConfig_t  faders[NUM_FADERS];
    uint8_t        active_preset;
} DeviceConfig_t;

// ========================= PUBLIC FUNC. DECL. ========================

extern DeviceConfig_t g_config;

#endif /* CONFIG_H */
