#include "mux.h"



/* PROTOTYPES */

uint8_t Get_Mux(uint8_t fader)
{
    return fader / 8;
}

uint8_t Get_Select(uint8_t fader)
{
    return fader % 8;
}

void Start_Mux()
{
    HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_RESET);
}

void Stop_Mux()
{
    HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_SET);
}