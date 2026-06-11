/* =========================================================
 * scheduler.c
 * Resource Manager layer — Simple cooperative round-robin scheduler
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "scheduler.h"
#include "config.h"
#include "faderbay_types.h"
#include "nlog.h"
#include <stdint.h>
#include <string.h>

#if SCHEDULER_PROFILING_ENABLE
#include "stm32f4xx_hal.h"
#endif

// ============================ PRIVATE TYPES ==========================

typedef struct {
    const char  *name;
    void       (*fn)(void);
    uint32_t     period_ms;
    uint32_t     last_run;
#if SCHEDULER_PROFILING_ENABLE
    SchedulerTaskStats_t stats;
#endif
} SchTask_t;

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "SCH";

static volatile uint32_t s_tick;
static SchTask_t s_tasks[SCH_MAX_TASKS];
static uint8_t   s_task_count;

// ========================= PRIVATE FUNC. DECL. =======================

static void exec_task(uint8_t index);

// =========================== PRIVATE FUNCTIONS =======================

static void exec_task(uint8_t index) {
    if (s_tasks[index].fn == NULL) return;

#if SCHEDULER_PROFILING_ENABLE
    uint32_t t0       = DWT->CYCCNT;
    s_tasks[index].fn();
    uint32_t dt_cycles = DWT->CYCCNT - t0;  /* unsigned subtraction handles wrap at ~51 s */

    uint32_t dt_us = dt_cycles / (SystemCoreClock / 1000000u);

    SchedulerTaskStats_t *st = &s_tasks[index].stats;
    st->last_us = dt_us;
    if (dt_us > st->max_us)                           st->max_us = dt_us;
    if (dt_us > s_tasks[index].period_ms * 1000u)     st->overrun_count++;
    st->total_runs++;
#else
    s_tasks[index].fn();
#endif

    s_tasks[index].last_run = s_tick;
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t Scheduler_Init(void) {
    s_tick = 0;
    memset(s_tasks, 0, sizeof(s_tasks));

#if SCHEDULER_PROFILING_ENABLE
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT       = 0;
    DWT->CTRL        |= DWT_CTRL_CYCCNTENA_Msk;
#endif

    LOGI(TAG, "Init OK");
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

fb_err_t Scheduler_RegisterTask(const char *name, void (*fn)(void), uint32_t period_ms) {
    if (fn == NULL || period_ms == 0) {
        LOGE(TAG, "RegisterTask: invalid param");
        return FB_ERR_INVALID_PARAM;
    }
    if (s_task_count >= SCH_MAX_TASKS) {
        LOGE(TAG, "RegisterTask: task table full");
        return FB_ERR_GENERIC;
    }

    s_tasks[s_task_count].name      = name;
    s_tasks[s_task_count].fn        = fn;
    s_tasks[s_task_count].period_ms = period_ms;
    s_tasks[s_task_count].last_run  = 0;
    s_task_count++;

    return FB_OK;
}

void Scheduler_Tick(void) {
    s_tick++;
}

fb_err_t Scheduler_GetStats(uint8_t task_index, SchedulerTaskStats_t *out) {
    if (out == NULL || task_index >= s_task_count) return FB_ERR_INVALID_PARAM;
#if SCHEDULER_PROFILING_ENABLE
    *out = s_tasks[task_index].stats;
#else
    memset(out, 0, sizeof(*out));
#endif
    return FB_OK;
}

void Scheduler_ResetStats(void) {
#if SCHEDULER_PROFILING_ENABLE
    for (uint8_t i = 0; i < s_task_count; i++) {
        memset(&s_tasks[i].stats, 0, sizeof(s_tasks[i].stats));
    }
#endif
}

void Scheduler_DumpStats(void) {
#if SCHEDULER_PROFILING_ENABLE
    LOGI(TAG, "stats:");
    for (uint8_t i = 0; i < s_task_count; i++) {
        SchTask_t *t = &s_tasks[i];
        LOGI(TAG, "  %s last=%lu max=%lu over=%lu runs=%lu period=%lums",
             t->name ? t->name : "?",
             (unsigned long)t->stats.last_us,
             (unsigned long)t->stats.max_us,
             (unsigned long)t->stats.overrun_count,
             (unsigned long)t->stats.total_runs,
             (unsigned long)t->period_ms);
    }
#else
    LOGI(TAG, "profiling disabled");
#endif
}

uint8_t Scheduler_GetTaskCount(void) {
    return s_task_count;
}
