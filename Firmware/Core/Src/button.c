#include "button.h"


/* API */



uint8_t Button_Get_State(Button_t *button) {
    uint8_t rawState = HAL_GPIO_ReadPin(button->port, button->pin); // Leer botón

    if (rawState != button->lastState) {
        button->debounceCounter = 0;  // Resetear debounce si cambia el estado
    } 
    else {
        if (button->debounceCounter < DEBOUNCE_THRESHOLD) {
            button->debounceCounter++;
        } 
        else {
            if (rawState != button->isPressed) {
                button->isPressed = rawState;

                if (button->isPressed == 0) { // Detectar flanco de bajada (botón presionado)
                    button->pressedFlag = 1;
                }
            }
        }
    }

    button->lastState = rawState;  // Guardar estado actual
    return button->pressedFlag;    // Retorna 1 si se detectó un cambio de estado
}
