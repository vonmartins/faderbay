#ifndef SCHEDULER_H
#define SCHEDULER_H

// ============================= INCLUDES ==============================

#include <stdint.h>
#include "faderbay_types.h"

// ============================ PUBLIC TYPES ===========================

typedef struct {
    uint32_t last_us;
    uint32_t max_us;
    uint32_t overrun_count;
    uint32_t total_runs;
} SchedulerTaskStats_t;

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t Scheduler_Init(void);
void     Scheduler_Run(void);
uint32_t Scheduler_GetTick(void);
fb_err_t Scheduler_RegisterTask(const char *name, void (*fn)(void), uint32_t period_ms);
void     Scheduler_Tick(void);

fb_err_t Scheduler_GetStats(uint8_t task_index, SchedulerTaskStats_t *out);
void     Scheduler_ResetStats(void);
void     Scheduler_DumpStats(void);
uint8_t  Scheduler_GetTaskCount(void);

#endif /* SCHEDULER_H */
