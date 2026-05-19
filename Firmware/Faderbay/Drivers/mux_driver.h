#ifndef MUX_DRIVER_H
#define MUX_DRIVER_H

// ============================= INCLUDES ==============================

#include <stdint.h>
#include "faderbay_types.h"
#include "gpio.h"

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

typedef struct {
    GPIO_TypeDef  *sel_ports[4];  /* Puertos de los pines S0–S3 */
    uint16_t       sel_pins[4];   /* Pines S0–S3 */
    uint8_t        num_sel_pins;  /* Número de pines de selección (1–4) */
    uint8_t        num_channels;  /* Número de canales (2, 4, 8 o 16) */
} MuxConfig_t;

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t MuxDriver_Init(const MuxConfig_t *config);

fb_err_t MuxDriver_SelectChannel(uint8_t channel);

#endif /* MUX_DRIVER_H */
