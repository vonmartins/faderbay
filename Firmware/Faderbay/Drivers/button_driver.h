#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

// ============================= INCLUDES ==============================

#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

typedef enum {
    BTN_EVT_NONE         = 0,
    BTN_EVT_PRESSED,
    BTN_EVT_RELEASED,
    BTN_EVT_LONG_PRESS,
    BTN_EVT_DOUBLE_CLICK,
} ButtonEventType_t;

typedef struct {
    uint8_t            index;         /* 0..NUM_BUTTONS-1 */
    ButtonEventType_t  type;
    uint32_t           timestamp_ms;  /* Scheduler_GetTick() at event time */
} ButtonEvent_t;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    bool          active_low;         /* true → pin LOW = pressed (hardware pull-up) */
    uint16_t      long_press_ms;      /* 0 disables long-press detection */
    bool          enable_double_click;
} ButtonConfig_t;

// ========================= PUBLIC FUNC. DECL. ========================

/* Pass configs=NULL, count=0 to use built-in Faderbay defaults (BTN_1..4, active-low). */
fb_err_t ButtonDriver_Init(const ButtonConfig_t *configs, uint8_t count);

/* Called by scheduler every PERIOD_BUTTON_DRIVER ms. Runs all state machines. */
void ButtonDriver_Process(void);

/* FIFO pop — returns false when the queue is empty. */
bool ButtonDriver_PollEvent(ButtonEvent_t *out_event);

/* True while the button's state machine is in PRESSED or LONG_FIRED state. */
bool ButtonDriver_IsPressed(uint8_t index);

#endif /* BUTTON_DRIVER_H */
