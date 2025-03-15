/* Fns prototypes, struct, defines */

#ifndef _FADER_H
#define _FADER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "device_def.h"

/* DEFINES */

#define MAX_MAPPED_VALUE    127
#define MIN_MAPPED_VALUE    0
#define MAX_RAW_VALUE       4095
#define MIN_RAW_VALUE       0

#define FADERS_ADC_HANDLE   ADC1

#define MUX1    0
#define MUX2    1


#define NUM_FADERS          16
#define FADER_DEFAULT       0

#define FADER_1     0
#define FADER_2     1
#define FADER_3     2
#define FADER_4     3
#define FADER_5     4
#define FADER_6     5
#define FADER_7     6
#define FADER_8     7
#define FADER_9     8
#define FADER_10    9
#define FADER_11    10
#define FADER_12    11
#define FADER_13    12
#define FADER_14    13
#define FADER_15    14
#define FADER_16    15

// #define PERIPH_BASE           0x40000000UL /*!< Peripheral base address in the alias region                                */
// #define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
// #define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
// #define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
// #define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)
// #define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00UL)
// #define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000UL)
// #define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00UL)
// #define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
// #define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
// #define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
// #define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
// #define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
// #define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
// #define MUX1_S0_Pin GPIO_PIN_7
// #define MUX1_S0_GPIO_Port GPIOA
// #define MUX1_S1_Pin GPIO_PIN_4
// #define MUX1_S1_GPIO_Port GPIOC
// #define MUX1_S2_Pin GPIO_PIN_5
// #define MUX1_S2_GPIO_Port GPIOC
// #define MUX2_S0_Pin GPIO_PIN_0
// #define MUX2_S0_GPIO_Port GPIOB
// #define MUX2_S1_Pin GPIO_PIN_1
// #define MUX2_S1_GPIO_Port GPIOB
// #define MUX2_S2_Pin GPIO_PIN_2
// #define MUX2_S2_GPIO_Port GPIOB
// #define MUX1_INH_Pin GPIO_PIN_11
// #define MUX1_INH_GPIO_Port GPIOE
// #define MUX2_INH_Pin GPIO_PIN_12
// #define MUX2_INH_GPIO_Port GPIOE




/* STRUCTS */

typedef struct {
    uint8_t name;
    // uint8_t fader_changed; /* Flag to process MIDI. Put to 0 after processing the fader */
    uint32_t raw_value; /* 0 - 4096 (12 bits ADC) */
    uint32_t last_raw_value;
    uint8_t smooth_value; /* 0 - 127 (MIDI) */
    uint8_t last_smooth_value;
    ADC_HandleTypeDef *hadc;
    uint8_t mux;
    uint8_t mux_channel;
} Fader_t;


/* API PROTOTYPES */

void Smooth_Fader(Fader_t *fader);
device_error_t Fader_Poll(Fader_t *fader);
uint16_t Fader_Get_Raw_Value(Fader_t *fader);
uint8_t Fader_Get_Smooth_Value(Fader_t *fader);
void Set_Faders_On();  /* Set LOW MUX1_INH and MUX2_INH pins */
void Set_Faders_Off();  /* Set HIGH MUX1_INH and MUX2_INH pins */




#endif