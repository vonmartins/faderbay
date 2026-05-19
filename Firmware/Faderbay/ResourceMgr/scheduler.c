/* =========================================================
 * scheduler.c
 * Resource Manager layer — Simple cooperative round-robin scheduler
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "scheduler.h"
#include "config.h"
#include "faderbay_types.h"
#include <stdint.h>
#include <string.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

typedef struct {
    uint32_t  period_ms;   /* Período de ejecución en ms */
    uint32_t  last_run;    /* Tick de la última ejecución */
    void    (*task)(void); /* Puntero a la función de proceso */
} SchTask_t;

// =========================== PRIVATE VARIABLES =======================

static volatile uint32_t s_tick;

static SchTask_t s_tasks[SCH_MAX_TASKS];

static uint8_t s_task_count;

// ========================= PRIVATE FUNC. DECL. =======================

static void exec_task(uint8_t index);

// =========================== PRIVATE FUNCTIONS =======================

static void exec_task(uint8_t index) {
    if (s_tasks[index].task == NULL) return;
    s_tasks[index].task();
    s_tasks[index].last_run = s_tick;
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t Scheduler_Init(void) {
    s_tick = 0;
    memset(s_tasks, 0, sizeof(s_tasks));
    return FB_OK;
}

void Scheduler_Run(void) {
    for (uint8_t i = 0; i < s_task_count; i++) {
        if (s_tick - s_tasks[i].last_run >= s_tasks[i].period_ms) {
            exec_task(i);
        }
    }
}

uint32_t Scheduler_GetTick(void) {

    return s_tick;
}

fb_err_t Scheduler_RegisterTask(void (*task)(void), uint32_t period_ms) {
    if (task == NULL || period_ms == 0) return FB_ERR_INVALID_PARAM;
    if (s_task_count >= SCH_MAX_TASKS) return FB_ERR_GENERIC;

    s_tasks[s_task_count].period_ms = period_ms;
    s_tasks[s_task_count].task = task;
    s_tasks[s_task_count].last_run = 0;
    s_task_count++;

    return FB_OK;
}

void Scheduler_Tick(void) {
    s_tick++;
}
