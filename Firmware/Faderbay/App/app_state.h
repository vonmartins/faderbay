#ifndef APP_STATE_H
#define APP_STATE_H

#include "faderbay_types.h"
#include <stdint.h>

fb_err_t AppState_Init(void);

AppState_t AppState_Get(void);

void AppState_Set(AppState_t state);

#endif /* APP_STATE_H */
