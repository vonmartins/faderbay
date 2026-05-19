/* =========================================================
 * button_driver.c
 * Driver layer — Debounced button input driver
 * ========================================================= */

#include "button_driver.h"
#include "config.h"
#include "faderbay_types.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>
#include "main.h"

static uint8_t s_state[NUM_BUTTONS];
static uint8_t s_event[NUM_BUTTONS];
static GPIO_TypeDef * const s_ports[NUM_BUTTONS] = {BTN_1_GPIO_Port, BTN_2_GPIO_Port, BTN_3_GPIO_Port, BTN_4_GPIO_Port};
static const uint16_t       s_pins[NUM_BUTTONS]  = {BTN_1_Pin, BTN_2_Pin, BTN_3_Pin, BTN_4_Pin};
static uint8_t s_debounce[NUM_BUTTONS];

fb_err_t ButtonDriver_Init(void) {
    memset(s_state, 0, sizeof(s_state));
    memset(s_event, 0, sizeof(s_event));
    memset(s_debounce, 0, sizeof(s_debounce));
    return FB_OK;
}

void ButtonDriver_Process(void) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        s_state[i] = GPIO_Get(s_ports[i], s_pins[i]);
        if (s_state[i] == BTN_PRESSED) {
            s_debounce[i]++;
            if (s_debounce[i] == 2) {
                s_event[i] = 1;
            }
        } else {
            s_debounce[i] = 0;
        }
    }
}

uint8_t ButtonDriver_GetEvent(uint8_t index) {
    if (s_event[index]) {
        s_event[index] = 0;
        return 1;
    } else return 0;
}
