#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "faderbay_types.h"

// ============================ PUBLIC DEFINES =========================

#define NUM_FADERS              16
#define NUM_BUTTONS              4
#define NUM_PRESETS             16
#define SCH_MAX_TASKS           10u

#define ADC_MAX_REAL            4000   /* was 3890; some faders need higher cap to reach 127 */
#define FADER_CHANGE_THRESHOLD  1

#define FADER_FILTER_SHIFT      3    /* IIR: y += (x - y) >> shift; shift=3 → ~Fs/16 cutoff */
#define FADER_MUX_SETTLE_US     10   /* µs to wait after MUX switch before sampling */
#define FADER_ADC_STALL_MS      50   /* task ticks before watchdog restarts ADC cycle */

#define ENCODER_COUNTS_PER_DETENT   4  /* quadrature counts per mechanical detent (TI1+TI2 mode) */
#define ENCODER_ACCEL_ENABLE        1  /* 0 = disable velocity acceleration */
#define ENCODER_ACCEL_FAST_MS       20 /* Δt below this → fast multiplier */
#define ENCODER_ACCEL_MED_MS        50 /* Δt below this → medium multiplier */
#define ENCODER_ACCEL_FAST_MULT     4  /* multiplier when spinning fast */
#define ENCODER_ACCEL_MED_MULT      2  /* multiplier when spinning at medium speed */

#define BUTTON_WARMUP_TICKS      10   /* discard first N ticks after Init to avoid spurious events from GPIO glitches at boot (100 ms @ PERIOD_BUTTON_DRIVER=10) */
#define BUTTON_DEBOUNCE_SAMPLES   2   /* consecutive equal readings to confirm state change */
#define BUTTON_LONG_PRESS_MS      500 /* ms held before BTN_EVT_LONG_PRESS fires */
#define BUTTON_DOUBLE_CLICK_MS    300 /* ms window after release to detect second press */
#define BUTTON_EVENT_QUEUE_SIZE   16  /* ring-buffer depth for pending button events */

#define BTN_MODE                0
#define BTN_SELECT              1
#define BTN_BACK                2
#define BTN_PARAM               3

#define UART_TX_RING_SIZE     128u  /* MIDI TX ring buffer size in bytes (must be power of 2) */

#define MIDI_DEFAULT_CHANNEL    1
#define MIDI_BASE_CC            7

// Scheduler profiling
#define SCHEDULER_PROFILING_ENABLE   1  /* set to 0 to remove DWT measurement overhead */
#define SCHEDULER_AUTODUMP_ENABLE    1
#define SCHEDULER_AUTODUMP_PERIOD_MS 30000

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
