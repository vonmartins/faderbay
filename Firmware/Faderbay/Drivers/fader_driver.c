/* =========================================================
 * fader_driver.c
 * Driver layer — Low-level motorised fader driver
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "fader_driver.h"
#include "config.h"
#include "faderbay_types.h"
#include "main.h"
#include "mux_driver.h"
#include "nlog.h"
#include <stdint.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

#define MUX_NUM_CH      NUM_FADERS
#define MUX_NUM_SEL     4

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "FADER_DRV";

static ADC_HandleTypeDef * s_hadc;
static uint16_t s_raw[NUM_FADERS];
static uint8_t s_channel;

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t FaderDriver_Init(ADC_HandleTypeDef *hadc) {
    if (hadc == NULL) {
        LOGE(TAG, "Init failed: null ADC handle");
        return FB_ERR_INVALID_PARAM;
    }
    memset(s_raw, 0, sizeof(s_raw));
    s_channel = 0;
    s_hadc = hadc;

    const MuxConfig_t s_mux_cfg = {
        .num_channels = MUX_NUM_CH,
        .num_sel_pins = MUX_NUM_SEL,
        .sel_pins = {MUX_S0_Pin, MUX_S1_Pin, MUX_S2_Pin, MUX_S3_Pin},
        .sel_ports = {MUX_S0_GPIO_Port, MUX_S1_GPIO_Port, MUX_S2_GPIO_Port, MUX_S3_GPIO_Port}
    };

    fb_err_t err = MuxDriver_Init(&s_mux_cfg);
    if (err != FB_OK) {
        LOGE(TAG, "Init failed: MuxDriver");
        return err;
    }
    LOGI(TAG, "Init OK");
    return FB_OK;
}

void FaderDriver_Process(void) {
    uint32_t value = 0;
    MuxDriver_SelectChannel(s_channel);
    if (ADC_StartConversion(s_hadc) == FB_OK) {
        if (ADC_GetValue(s_hadc, &value) == FB_OK) {
            s_raw[s_channel] = (uint16_t)value;
        }
    }
    s_channel = (s_channel + 1) % NUM_FADERS;
}

uint16_t FaderDriver_GetRaw(uint8_t index) {
    if (index >= NUM_FADERS) return 0;
    return s_raw[index];
}
