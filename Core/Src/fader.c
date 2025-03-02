/* Fader API (Read, MUX, etc) */


#include "fader.h"
#include "app_config.h"

//#include "UI.h"
//#include "stm32f4xx_hal_adc.h"


/* VARIABLES */



/* API */

// void Select_Mux(uint8_t mux, uint8_t channel)
// {
//     if (channel > 7) return;

//     if (mux == MUX1)
//     {
//         HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_SET);

//         HAL_GPIO_WritePin(MUX1_S0_GPIO_Port, MUX1_S0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX1_S1_GPIO_Port, MUX1_S1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX1_S2_GPIO_Port, MUX1_S2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     }
//     else if (mux == MUX2)
//     {
//         HAL_GPIO_WritePin(MUX2_INH_GPIO_Port, MUX2_INH_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX1_INH_GPIO_Port, MUX1_INH_Pin, GPIO_PIN_SET);

//         HAL_GPIO_WritePin(MUX2_S0_GPIO_Port, MUX2_S0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX2_S1_GPIO_Port, MUX2_S1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MUX2_S2_GPIO_Port, MUX2_S2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     }

//     HAL_Delay(1); /* Wait for mux select */
// }


void Smooth_Fader(Fader_t *fader)
{
    fader->smooth_value = (uint8_t)(
        ( (MAX_MAPPED_VALUE - MIN_MAPPED_VALUE) * (fader->raw_value - MIN_RAW_VALUE) ) 
        / (MAX_RAW_VALUE - MIN_RAW_VALUE)
    );
}


uint16_t Faders_Get_Raw_Value(Fader_t *faders, uint8_t fader)
{
    if (fader >= NUM_FADERS) {
        return 0; // O un valor de error
    }

    ADC_HandleTypeDef *hadc = faders[fader].hadc;

    if (hadc == NULL) {
        return 0; 
    }

    Select_Mux(faders[fader].mux, faders[fader].mux_channel);
    HAL_Delay(1);

    HAL_ADC_Start(hadc);

    if (HAL_ADC_PollForConversion(hadc, 10) != HAL_OK) {
        return 0; 
    }

    return HAL_ADC_GetValue(hadc);
}


uint8_t Faders_Get_Smooth_Value(Fader_t *faders, uint8_t fader)
{
    Smooth_Fader(&faders[fader]);
    return faders[fader].smooth_value;
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