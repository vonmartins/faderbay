#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

uint32_t Timer_Now(void);
uint8_t  Timer_Elapsed(uint32_t start, uint32_t duration_ms);
void     Timer_Delay(uint32_t duration_ms);

#endif /* TIMER_H */
