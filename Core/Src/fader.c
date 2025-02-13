/* Fader API (Read, MUX, etc) */

#include <stdint.h>
#include "fader.h"
#include "app_config.h"
#include "stm32f4xx_hal_adc.h"


/* VARIABLES */

extern ADC_HandleTypeDef hadc1;


/* API */

void Smooth_Fader(Fader_t *fader)
{
    fader->smooth_value = (uint8_t)(
        ( (MAX_MAPPED_VALUE - MIN_MAPPED_VALUE) * (fader->raw_value - MIN_RAW_VALUE) ) 
        / (MAX_RAW_VALUE - MIN_RAW_VALUE)
    );
}

/* Return fader changed or -1 if no fader changed */
int8_t Faders_Update(Fader_t *faders)
{
    int8_t changed_fader = -1;

    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        uint8_t mux = Get_Mux(i);       // Obtener qué MUX usar (0 o 1)
        uint8_t select = Get_Select(i); // Obtener canal dentro del MUX (0-7)

        // Seleccionar canal en el MUX
        MUX_SelectChannel(select);
        HAL_Delay(1);  // Pequeña espera para estabilizar

        // Choose ADC
        uint32_t raw_value;
        if (mux == MUX1)
        {
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            raw_value = HAL_ADC_GetValue(&hadc1);
        }
        else
        {
            HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
            raw_value = HAL_ADC_GetValue(&hadc2);
        }

        // If value changed
        if (faders[i].raw_value != raw_value)
        {
            faders[i].raw_value = raw_value;
            Smooth_Fader(&faders[i]);
            changed_fader = i;
        }
    }
    return changed_fader;  // -1 si ningún fader cambió
}
HAL_ADC_PollForConversi

