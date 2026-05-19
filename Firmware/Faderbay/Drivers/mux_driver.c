/* =========================================================
 * mux_driver.c
 * Driver layer — Multiplexor driver
 * ========================================================= */

#include "mux_driver.h"
#include "faderbay_types.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>

static MuxConfig_t s_config;
static uint8_t s_initialized;

fb_err_t MuxDriver_Init(const MuxConfig_t *config) {
    if (config == NULL) return FB_ERR_INVALID_PARAM;
    if (config->num_channels == 0 || (config->num_channels & (config->num_channels - 1)) != 0) {
        return FB_ERR_INVALID_PARAM;
    }
    s_config.num_channels = config->num_channels;
    s_config.num_sel_pins = config->num_sel_pins;
    memcpy(s_config.sel_pins,  config->sel_pins,  sizeof(s_config.sel_pins));
    memcpy(s_config.sel_ports, config->sel_ports, sizeof(s_config.sel_ports));
    s_initialized = 1;
    return FB_OK;
}

fb_err_t MuxDriver_SelectChannel(uint8_t channel) {
    if (!s_initialized) return FB_ERR_NOT_READY;
    if (channel >= s_config.num_channels) return FB_ERR_INVALID_PARAM;
    for (uint8_t i = 0; i < s_config.num_sel_pins; i++) {
        GPIO_Set(s_config.sel_ports[i], s_config.sel_pins[i], (channel >> i) & 1);
    }
    return FB_OK;
}
