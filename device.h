#include "fader.h"
#include "encoder.h"
#include "button.h"

/* DEFINES */

 #define NUM_FADERS     16
 #define NUM_BUTTONS     4

/* STRUCTS */

typedef struct {
    Fader_t faders[NUM_FADERS];
    Button_t buttons[NUM_BUTTONS];
    Encoder_t encoder;

} device_t;

/* PROTOTYPES */



