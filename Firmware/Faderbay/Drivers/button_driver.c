/* =========================================================
 * button_driver.c
 * Driver layer — Debounced button driver with state machine and event queue
 *
 * NOTE: BTN_1..4 GPIO pins are configured as EXTI in CubeMX (GPIO_MODE_IT_RISING).
 * Those EXTI lines are RESERVED for a future wake-from-stop implementation.
 * This driver intentionally ignores EXTI and uses cooperative polling only —
 * it is more robust against contact bounce and RF noise than edge-triggered EXTI.
 *
 * STATE MACHINE (per button)
 * ==========================
 *
 *   S_IDLE
 *     on DEBOUNCE_SAMPLES consecutive PRESSED reads
 *       → emit BTN_EVT_PRESSED, enter S_PRESSED
 *
 *   S_PRESSED   (long-press timer runs here)
 *     on DEBOUNCE_SAMPLES consecutive RELEASED reads
 *       → emit BTN_EVT_RELEASED
 *       → if enable_double_click: enter S_WAIT_DOUBLE_CLICK
 *       → else: enter S_IDLE
 *     on hold_ticks >= lp_ticks (and long_press_ms > 0)
 *       → emit BTN_EVT_LONG_PRESS, enter S_LONG_FIRED
 *     transient RELEASED (<DEBOUNCE_SAMPLES): release_debounce resets, stay in S_PRESSED
 *
 *   S_LONG_FIRED   (button still held after long press)
 *     on DEBOUNCE_SAMPLES consecutive RELEASED reads
 *       → emit BTN_EVT_RELEASED
 *       → if enable_double_click: enter S_WAIT_DOUBLE_CLICK
 *       → else: enter S_IDLE
 *     no re-emit on continued hold (repeat is a future feature)
 *
 *   S_WAIT_DOUBLE_CLICK   (only when enable_double_click == true)
 *     on DEBOUNCE_SAMPLES consecutive PRESSED reads within DOUBLE_CLICK_MS
 *       → emit BTN_EVT_DOUBLE_CLICK, enter S_IDLE
 *     on timeout (dc_timer > DOUBLE_CLICK_TICKS)
 *       → enter S_IDLE (second press missed or not expected)
 *
 * QUEUE
 * =====
 * Ring buffer of BUTTON_EVENT_QUEUE_SIZE entries. If full, the new event is
 * discarded and a LOGW is emitted. The queue lives entirely in the main loop
 * (no ISR writers), so no critical section is needed.
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "button_driver.h"
#include "config.h"
#include "faderbay_types.h"
#include "gpio.h"
#include "main.h"
#include "nlog.h"
#include "scheduler.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

#define DOUBLE_CLICK_TICKS  ((uint16_t)(BUTTON_DOUBLE_CLICK_MS / PERIOD_BUTTON_DRIVER))

// ============================ PRIVATE TYPES ==========================

typedef enum {
    S_IDLE,
    S_PRESSED,
    S_LONG_FIRED,
    S_WAIT_DOUBLE_CLICK,
} BtnState_t;

typedef struct {
    ButtonConfig_t  cfg;
    BtnState_t      state;
    uint8_t         debounce_cnt;      /* counts consecutive PRESSED reads (IDLE→PRESSED) */
    uint8_t         release_debounce;  /* counts consecutive RELEASED reads (PRESSED/LONG_FIRED) */
    uint16_t        hold_ticks;        /* ticks held in S_PRESSED; resets on entry */
    uint16_t        lp_ticks;          /* precomputed: cfg.long_press_ms / PERIOD_BUTTON_DRIVER */
    uint16_t        dc_timer;          /* ticks elapsed in S_WAIT_DOUBLE_CLICK */
} BtnCtx_t;

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "BTN_DRV";

static BtnCtx_t s_ctx[NUM_BUTTONS];
static uint8_t  s_btn_count;

static ButtonEvent_t s_eq_buf[BUTTON_EVENT_QUEUE_SIZE];
static uint8_t       s_eq_head;
static uint8_t       s_eq_tail;
static uint8_t       s_eq_count;

/* Default hardware config for the 4 Faderbay buttons.
 * Active-low (external pull-up resistors on PCB).
 * Double-click disabled by default to avoid the inherent latency it adds. */
static const ButtonConfig_t s_default_configs[NUM_BUTTONS] = {
    { BTN_1_GPIO_Port, BTN_1_Pin, true, BUTTON_LONG_PRESS_MS, false },
    { BTN_2_GPIO_Port, BTN_2_Pin, true, BUTTON_LONG_PRESS_MS, false },
    { BTN_3_GPIO_Port, BTN_3_Pin, true, BUTTON_LONG_PRESS_MS, false },
    { BTN_4_GPIO_Port, BTN_4_Pin, true, BUTTON_LONG_PRESS_MS, false },
};

// ========================= PRIVATE FUNC. DECL. =======================

static uint8_t  read_raw(const BtnCtx_t *ctx);
static void     push_event(uint8_t idx, ButtonEventType_t type);
static void     process_button(uint8_t i);

// =========================== PRIVATE FUNCTIONS =======================

/* Returns 1 if the button is physically pressed, 0 if released.
 * Handles active_low inversion so the rest of the driver is polarity-agnostic. */
static uint8_t read_raw(const BtnCtx_t *ctx)
{
    uint8_t pin = GPIO_Get(ctx->cfg.port, ctx->cfg.pin);
    return ctx->cfg.active_low ? (pin == 0u ? 1u : 0u) : (pin != 0u ? 1u : 0u);
}

/* Append an event to the ring buffer.
 * If full, discards the new event and logs a warning (non-blocking). */
static void push_event(uint8_t idx, ButtonEventType_t type)
{
    if (s_eq_count >= BUTTON_EVENT_QUEUE_SIZE) {
        LOGW(TAG, "Queue full, dropping event btn=%u type=%u", idx, (unsigned)type);
        return;
    }
    s_eq_buf[s_eq_head].index        = idx;
    s_eq_buf[s_eq_head].type         = type;
    s_eq_buf[s_eq_head].timestamp_ms = Scheduler_GetTick();
    s_eq_head = (uint8_t)((s_eq_head + 1u) % BUTTON_EVENT_QUEUE_SIZE);
    s_eq_count++;
}

/* Shared release-confirmed logic for S_PRESSED and S_LONG_FIRED. */
static void on_release_confirmed(uint8_t i)
{
    push_event(i, BTN_EVT_RELEASED);
    s_ctx[i].release_debounce = 0;
    s_ctx[i].debounce_cnt     = 0;
    if (s_ctx[i].cfg.enable_double_click) {
        s_ctx[i].dc_timer = 0;
        s_ctx[i].state    = S_WAIT_DOUBLE_CLICK;
    } else {
        s_ctx[i].state = S_IDLE;
    }
}

static void process_button(uint8_t i)
{
    BtnCtx_t *ctx = &s_ctx[i];
    uint8_t   pressed = read_raw(ctx);

    switch (ctx->state) {

    case S_IDLE:
        if (pressed) {
            ctx->debounce_cnt++;
            if (ctx->debounce_cnt >= (uint8_t)BUTTON_DEBOUNCE_SAMPLES) {
                ctx->debounce_cnt    = 0;
                ctx->hold_ticks      = 0;
                ctx->release_debounce = 0;
                push_event(i, BTN_EVT_PRESSED);
                ctx->state = S_PRESSED;
            }
        } else {
            ctx->debounce_cnt = 0;
        }
        break;

    case S_PRESSED:
        if (pressed) {
            /* Button held — count toward long press, reset release debounce. */
            ctx->release_debounce = 0;
            ctx->hold_ticks++;
            if (ctx->lp_ticks > 0u && ctx->hold_ticks >= ctx->lp_ticks) {
                push_event(i, BTN_EVT_LONG_PRESS);
                ctx->state = S_LONG_FIRED;
            }
        } else {
            /* Possible release — debounce it; transient bounces stay in S_PRESSED. */
            ctx->release_debounce++;
            if (ctx->release_debounce >= (uint8_t)BUTTON_DEBOUNCE_SAMPLES) {
                on_release_confirmed(i);
            }
        }
        break;

    case S_LONG_FIRED:
        if (!pressed) {
            ctx->release_debounce++;
            if (ctx->release_debounce >= (uint8_t)BUTTON_DEBOUNCE_SAMPLES) {
                on_release_confirmed(i);
            }
        } else {
            ctx->release_debounce = 0;
        }
        break;

    case S_WAIT_DOUBLE_CLICK:
        ctx->dc_timer++;
        /* Check timeout before press debounce to avoid a late second-click
         * being mistakenly absorbed after the window has already closed. */
        if (ctx->dc_timer > DOUBLE_CLICK_TICKS) {
            ctx->debounce_cnt = 0;
            ctx->state = S_IDLE;
            break;
        }
        if (pressed) {
            ctx->debounce_cnt++;
            if (ctx->debounce_cnt >= (uint8_t)BUTTON_DEBOUNCE_SAMPLES) {
                push_event(i, BTN_EVT_DOUBLE_CLICK);
                ctx->debounce_cnt = 0;
                ctx->state = S_IDLE;
            }
        } else {
            ctx->debounce_cnt = 0;
        }
        break;
    }
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t ButtonDriver_Init(const ButtonConfig_t *configs, uint8_t count)
{
    const ButtonConfig_t *src   = (configs != NULL) ? configs : s_default_configs;
    uint8_t               n     = (configs != NULL) ? count   : (uint8_t)NUM_BUTTONS;

    if (n == 0u || n > (uint8_t)NUM_BUTTONS) {
        LOGE(TAG, "Init: invalid count %u", n);
        return FB_ERR_INVALID_PARAM;
    }

    memset(s_ctx,    0, sizeof(s_ctx));
    memset(s_eq_buf, 0, sizeof(s_eq_buf));
    s_eq_head  = 0;
    s_eq_tail  = 0;
    s_eq_count = 0;
    s_btn_count = n;

    for (uint8_t i = 0; i < n; i++) {
        s_ctx[i].cfg   = src[i];
        s_ctx[i].state = S_IDLE;
        /* Precompute long-press tick threshold to avoid per-tick integer division. */
        s_ctx[i].lp_ticks = (src[i].long_press_ms > 0u)
                            ? (uint16_t)(src[i].long_press_ms / PERIOD_BUTTON_DRIVER)
                            : 0u;
    }

    LOGI(TAG, "Init OK (%u buttons)", n);
    return FB_OK;
}

void ButtonDriver_Process(void)
{
    for (uint8_t i = 0; i < s_btn_count; i++) {
        process_button(i);
    }
}

bool ButtonDriver_PollEvent(ButtonEvent_t *out_event)
{
    if (s_eq_count == 0u) return false;
    *out_event = s_eq_buf[s_eq_tail];
    s_eq_tail  = (uint8_t)((s_eq_tail + 1u) % BUTTON_EVENT_QUEUE_SIZE);
    s_eq_count--;
    return true;
}

bool ButtonDriver_IsPressed(uint8_t index)
{
    if (index >= s_btn_count) return false;
    return (s_ctx[index].state == S_PRESSED || s_ctx[index].state == S_LONG_FIRED);
}
