/* =========================================================
 * app_state.c
 * Application layer — Top-level application state machine
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "app_state.h"
#include "faderbay_types.h"
#include "nlog.h"

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "APP_STATE";

static AppState_t s_state;

// ========================= PRIVATE FUNC. DECL. =======================

// =========================== PRIVATE FUNCTIONS =======================

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t AppState_Init(void) {
    s_state = UI_MODE_PERFORMANCE;
    LOGI(TAG, "Init OK");
    return FB_OK;
}

AppState_t AppState_Get(void) {
    return s_state;
}

void AppState_Set(AppState_t state) {
    s_state = state;
}
