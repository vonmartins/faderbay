#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "faderbay_types.h"



/* --- Public functions --------------------------------- */

fb_err_t Scheduler_Init(void);

void Scheduler_Run(void);

uint32_t Scheduler_GetTick(void);

fb_err_t Scheduler_RegisterTask(void (*task)(void), uint32_t period_ms);

void Scheduler_Tick(void);

/* --- Private functions --------------------------------- */


#endif /* SCHEDULER_H */
