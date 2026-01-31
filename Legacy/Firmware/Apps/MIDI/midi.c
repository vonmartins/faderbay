#include "midi.h"

uint8_t sendMidiCC(UART_HandleTypeDef *huart, midi_msg *msg)
{
    uint8_t pack[3] = {msg->header, msg->data1, msg->data2};

    if(HAL_UART_Transmit(huart, pack, sizeof(pack), HAL_MAX_DELAY) == HAL_OK) return 1;
    else return 0;

}