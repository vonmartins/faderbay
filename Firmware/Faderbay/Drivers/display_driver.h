#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

// ============================= INCLUDES ==============================

#include "faderbay_types.h"
#include "spi.h"
#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t DisplayDriver_Init(SPI_HandleTypeDef *hspi);

void DisplayDriver_Clear(void);

void DisplayDriver_DrawStr(uint8_t x, uint8_t y, const char *str);

void DisplayDriver_DrawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void DisplayDriver_Flush(void);

#endif /* DISPLAY_DRIVER_H */
