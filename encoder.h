/* Fns protoypes, struct, defines */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

/* DEFINES */

#define ENCODER_TIM     TIM2  /* ADJUST TIMER */

#define ENCODER_MIN_COUNTER 1
#define ENCODER_MAX_COUNTER 127 


/* STRUCTS */

typedef struct {
    uint8_t name;
    int32_t position;
    uint32_t max_value;
    uint32_t min_value;
} Encoder_t;


/* API PROTOTYPES */

// void Encoder_Init(Encoder_t *encoder, TIM_HandleTypeDef *htim, uint8_t name);
int32_t Encoder_GetPosition(Encoder_t *encoder);
void Encoder_SetPosition(Encoder_t *encoder, int32_t position);
uint8_t Encoder_UpdatePosition(Encoder_t *encoder);


#endif 