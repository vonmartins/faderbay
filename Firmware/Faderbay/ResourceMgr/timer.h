#ifndef TIMER_H
#define TIMER_H

// ============================= INCLUDES ==============================

#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

uint32_t Timer_Now(void);
uint8_t  Timer_Elapsed(uint32_t start, uint32_t duration_ms);
void     Timer_Delay(uint32_t duration_ms);

#endif /* TIMER_H */
