/* =========================================================
 * midi_control.c
 * Application layer — MIDI message processing and routing
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "midi_control.h"
#include "app_state.h"
#include "config.h"
#include "faderbay_types.h"
#include "fader_control.h"
#include "midi_driver.h"
#include "nlog.h"
#include <stdint.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "MIDI_CTRL";

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

void MidiControl_Process(void) {
    if (AppState_Get() != UI_MODE_PERFORMANCE) return;
    for (uint8_t i = 0; i < NUM_FADERS; i++) {
        FaderMode_t mode = g_config.faders[i].mode;
        if (mode != FADER_MODE_MIDI && mode != FADER_MODE_MIDI_CV) continue;
        if (FaderControl_HasChanged(i)) {
            uint8_t ch    = g_config.faders[i].midi_channel;
            uint8_t cc    = g_config.faders[i].midi_cc;
            uint8_t value = FaderControl_GetMidi(i);
            fb_err_t err = MidiDriver_SendCC(ch, cc, value);
            if (err != FB_OK) {
                LOGE(TAG, "SendCC failed: fader %u", i);
            } else {
                LOGD(TAG, "CC ch=%u cc=%u val=%u", ch, cc, value);
            }
        }
    }
}
