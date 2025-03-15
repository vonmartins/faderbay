#include "midi.h"

uint8_t sendMidiCC(UART_HandleTypeDef *huart, midi_msg *msg)
{
    // midi_msg msg = {
    //     .header = 0xb0 | (ch - 1),
    //     .data1 = cc,
    //     .data2 = val
    // };

    uint8_t pack[3] = {msg->header, msg->data1, msg->data2};

    if(HAL_UART_Transmit(huart, pack, sizeof(pack), HAL_MAX_DELAY) == HAL_OK) return 1;
    else return 0;
}