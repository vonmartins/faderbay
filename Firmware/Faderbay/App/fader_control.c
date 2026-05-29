/* =========================================================
 * fader_control.c
 * Application layer — Fader position and touch application logic
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "fader_control.h"
#include "fader_driver.h"
#include "faderbay_types.h"
#include "nlog.h"
#include <stdint.h>
#include <string.h>
#include "config.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "FADER_CTRL";

static uint8_t s_midi[NUM_FADERS];
static uint8_t s_changed[NUM_FADERS];

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t FaderControl_Init(void) {
    memset(s_midi, 0, sizeof(s_midi));
    memset(s_changed, 0, sizeof(s_changed));
    LOGI(TAG, "Init OK");
    return FB_OK;
}

void FaderControl_Process(void) {
    for (uint8_t i = 0; i < NUM_FADERS; i++) {
        uint16_t raw = FaderDriver_GetFiltered(i);
        if (raw > (uint16_t)ADC_MAX_REAL) { raw = (uint16_t)ADC_MAX_REAL; }
        uint8_t midi_val = (uint8_t)(((uint32_t)raw * 127u) / (uint32_t)ADC_MAX_REAL);
        uint8_t diff = (midi_val > s_midi[i]) ?
                       (midi_val - s_midi[i]) :
                       (s_midi[i] - midi_val);
        if (diff > FADER_CHANGE_THRESHOLD) {
            s_midi[i] = midi_val;
            s_changed[i] = 1;
        }
    }
}

uint8_t FaderControl_GetMidi(uint8_t index) {
    if (index >= NUM_FADERS) return 0;
    return s_midi[index];
}

uint8_t FaderControl_HasChanged(uint8_t index) {
    if (index >= NUM_FADERS) return 0;
    if (s_changed[index]) {
        s_changed[index] = 0;
        return 1;
    } else {
        return 0;
    }
}
