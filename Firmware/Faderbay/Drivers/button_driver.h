#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include "faderbay_types.h"
#include <stdint.h>

#define BTN_PRESSED     0
#define BTN_RELEASED    1

fb_err_t ButtonDriver_Init(void);

void ButtonDriver_Process(void);

uint8_t ButtonDriver_GetEvent(uint8_t index);

#endif /* BUTTON_DRIVER_H */
