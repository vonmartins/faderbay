/* Fns prototypes, struct, defines */

#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

/* DEFINES */




/* STRUCTS */

typedef struct {
    uint8_t debounceTime;
    uint8_t isPressed;
    /* TO DO */

} Button_t;


/* API PROTOTYPES */

void Button_Init();



#endif