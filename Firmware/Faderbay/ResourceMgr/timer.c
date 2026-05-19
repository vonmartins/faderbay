/* =========================================================
 * timer.c
 * Resource Manager layer — Non-blocking time utilities
 * ========================================================= */

#include "timer.h"
#include "scheduler.h"

uint32_t Timer_Now(void)
{
    return Scheduler_GetTick();
}

uint8_t Timer_Elapsed(uint32_t start, uint32_t duration_ms)
{
    return (Timer_Now() - start) >= duration_ms;
}

/* WARNING: busy-wait — never call from a scheduler task, only during init */
void Timer_Delay(uint32_t duration_ms)
{
    uint32_t start = Timer_Now();
    while (!Timer_Elapsed(start, duration_ms)) {}
}
