#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

// ============================= INCLUDES ==============================

#include "faderbay_types.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

// ========================= PUBLIC FUNC. DECL. ========================

fb_err_t EncoderDriver_Init(TIM_HandleTypeDef *htim);

/* Called by scheduler every PERIOD_ENCODER_DRIVER ms.
 * Reads TIM counter, accumulates raw counts, extracts whole detents,
 * and applies velocity acceleration. */
void EncoderDriver_Process(void);

/* Returns accumulated logical detents (with acceleration applied) since
 * the last call and resets the accumulator to zero.
 * INVARIANT: calling this does NOT reset the partial raw-count accumulator,
 * so sub-detent motion is never lost between calls. */
int16_t EncoderDriver_GetDelta(void);

/* Debug: returns raw TIM count delta since the last call and resets.
 * No detent division, no acceleration. Useful for diagnosing encoder
 * wiring, counts-per-detent calibration, and jitter measurement. */
int16_t EncoderDriver_GetRawDelta(void);

#endif /* ENCODER_DRIVER_H */
