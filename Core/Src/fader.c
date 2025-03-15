#include "fader.h"
#include "app_config.h"
#include "main.h"


/* VARIABLES */



/* API */

void Select_Mux(uint8_t mux, uint8_t channel)
{
    if (channel > 7) return;

    if (mux == MUX1)
    {
        HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(MUX1_S0_GPIO_Port, MUX1_S0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_S1_GPIO_Port, MUX1_S1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_S2_GPIO_Port, MUX1_S2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    else if (mux == MUX2)
    {
        HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(MUX2_S0_GPIO_Port, MUX2_S0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_S1_GPIO_Port, MUX2_S1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_S2_GPIO_Port, MUX2_S2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    HAL_Delay(1); /* Wait for mux select */
}


void Smooth_Fader(Fader_t *fader)
{
    fader->smooth_value = (uint8_t)(
        ( (MAX_MAPPED_VALUE - MIN_MAPPED_VALUE) * (fader->raw_value - MIN_RAW_VALUE) ) 
        / (MAX_RAW_VALUE - MIN_RAW_VALUE)
    );
}

device_error_t Fader_Poll(Fader_t *fader)
{
    if (fader == NULL) {
        return DEVICE_INVALID_PARAM;
    }

    Select_Mux(fader->mux, fader->mux_channel);
    HAL_Delay(1);
    HAL_ADC_Start(fader->hadc);
    if (HAL_ADC_PollForConversion(fader->hadc, 10) != HAL_OK) {
        return DEVICE_ADC_ERROR;
    }
    fader->raw_value = HAL_ADC_GetValue(fader->hadc);

    return DEVICE_OK;
}


uint16_t Fader_Get_Raw_Value(Fader_t *fader)
{
    return fader->raw_value;
}

uint8_t Fader_Get_Smooth_Value(Fader_t *fader)
{
    Smooth_Fader(fader);
    return fader->smooth_value;
}


// void Set_Faders_On()  /* Set LOW MUX1_INH and MUX2_INH pins */
// {
//     HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_RESET);
// }

// void Set_Faders_Off()  /* Set HIGH MUX1_INH and MUX2_INH pins */
// {
//     HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_SET);
//     HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_SET);
// }