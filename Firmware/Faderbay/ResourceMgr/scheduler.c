/* =========================================================
 * scheduler.c
 * Resource Manager layer — Simple cooperative round-robin scheduler
 * ========================================================= */

#include "scheduler.h"

/* --- Private types --------------------------------- */

typedef struct {
    uint32_t  period_ms;   /* Período de ejecución en ms */
    uint32_t  last_run;    /* Tick de la última ejecución */
    void    (*task)(void); /* Puntero a la función de proceso */
} SchTask_t;
