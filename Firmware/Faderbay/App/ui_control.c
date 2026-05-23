/* =========================================================
 * ui_control.c
 * Application layer — User interface event handling
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "ui_control.h"
#include "app_state.h"
#include "button_driver.h"
#include "config.h"
#include "display_driver.h"
#include "faderbay_types.h"
#include "encoder_driver.h"
#include "nlog.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "UI_CTRL";

static uint8_t        s_selected_fader;
static uint8_t        s_selected_param;
static FaderConfig_t  s_config_backup;

// ========================= PRIVATE FUNC. DECL. =======================

static uint8_t clamp(int16_t val, uint8_t min, uint8_t max);
static void draw_performance(void);
static void draw_editing(void);
static void draw_config(void);

// =========================== PRIVATE FUNCTIONS =======================

static uint8_t clamp(int16_t val, uint8_t min, uint8_t max) {
    if (val < min) return min;
    if (val > max) return max;
    return (uint8_t)val;
}

static void draw_performance(void) {
    char buf[24];
    DisplayDriver_DrawStr(0, 10, "FADERBAY  MIDI");
    uint8_t cc_first = g_config.faders[0].midi_cc;
    uint8_t cc_last  = g_config.faders[NUM_FADERS - 1].midi_cc;
    uint8_t ch       = g_config.faders[0].midi_channel;
    snprintf(buf, sizeof(buf), "CH:%u CC:%u-%u", ch, cc_first, cc_last);
    DisplayDriver_DrawStr(0, 28, buf);
}

static void draw_editing(void) {
    char buf[24];
    DisplayDriver_DrawStr(0, 10, "SELECT FADER");
    snprintf(buf, sizeof(buf), "> Fader %02u <", s_selected_fader + 1);
    DisplayDriver_DrawStr(0, 28, buf);
}

static void draw_config(void) {
    char buf[24];
    FaderConfig_t *f = &g_config.faders[s_selected_fader];

    snprintf(buf, sizeof(buf), "FADER %02u", s_selected_fader + 1);
    DisplayDriver_DrawStr(0, 10, buf);

    const char *mode_str = (f->mode == FADER_MODE_OFF)     ? "OFF"     :
                           (f->mode == FADER_MODE_MIDI)    ? "MIDI"    :
                           (f->mode == FADER_MODE_MIDI_CV) ? "MIDI+CV" : "CV";

    switch (s_selected_param) {
        case 0:
            snprintf(buf, sizeof(buf), ">CH:%u CC:%u %s",
                     f->midi_channel, f->midi_cc, mode_str);
            break;
        case 1:
            snprintf(buf, sizeof(buf), "CH:%u >CC:%u %s",
                     f->midi_channel, f->midi_cc, mode_str);
            break;
        case 2:
            snprintf(buf, sizeof(buf), "CH:%u CC:%u >%s",
                     f->midi_channel, f->midi_cc, mode_str);
            break;
    }
    DisplayDriver_DrawStr(0, 28, buf);
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t UIControl_Init(void) {
    s_selected_fader = 0;
    s_selected_param = 0;
    memset(&s_config_backup, 0, sizeof(s_config_backup));
    LOGI(TAG, "Init OK");
    return FB_OK;
}

void UIControl_Process(void) {
    DisplayDriver_Clear();

    switch (AppState_Get()) {
        case UI_MODE_PERFORMANCE:
            if (ButtonDriver_GetEvent(BTN_MODE)) {
                LOGI(TAG, "State -> EDITING");
                AppState_Set(UI_MODE_EDITING);
            }
            draw_performance();
            break;

        case UI_MODE_EDITING: {
            int16_t delta = EncoderDriver_GetDelta();
            if (delta != 0) {
                s_selected_fader = (s_selected_fader + delta + NUM_FADERS) % NUM_FADERS;
            }
            if (ButtonDriver_GetEvent(BTN_SELECT)) {
                s_config_backup = g_config.faders[s_selected_fader];
                LOGI(TAG, "State -> CONFIG");
                AppState_Set(UI_MODE_CONFIG);
            }
            if (ButtonDriver_GetEvent(BTN_BACK)) {
                LOGI(TAG, "State -> PERFORMANCE");
                AppState_Set(UI_MODE_PERFORMANCE);
            }
            draw_editing();
            break;
        }

        case UI_MODE_CONFIG: {
            int16_t delta = EncoderDriver_GetDelta();
            if (ButtonDriver_GetEvent(BTN_PARAM)) {
                s_selected_param = (s_selected_param + 1) % 3;
            } else if (ButtonDriver_GetEvent(BTN_SELECT)) {
                LOGI(TAG, "State -> PERFORMANCE");
                AppState_Set(UI_MODE_PERFORMANCE);
            } else if (ButtonDriver_GetEvent(BTN_BACK)) {
                g_config.faders[s_selected_fader] = s_config_backup;
                LOGI(TAG, "State -> EDITING");
                AppState_Set(UI_MODE_EDITING);
            }
            if (delta) {
                switch (s_selected_param) {
                    case 0:
                        g_config.faders[s_selected_fader].midi_channel = clamp(
                            g_config.faders[s_selected_fader].midi_channel + delta, 1, 16);
                        break;
                    case 1:
                        g_config.faders[s_selected_fader].midi_cc = clamp(
                            g_config.faders[s_selected_fader].midi_cc + delta, 0, 127);
                        break;
                    case 2:
                        g_config.faders[s_selected_fader].mode = (FaderMode_t)clamp(
                            g_config.faders[s_selected_fader].mode + delta, 0, 3);
                        break;
                }
            }
            draw_config();
            break;
        }
    }
}
