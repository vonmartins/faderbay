/* UI general API (Polling, Init, etc) */

/* DEFINES */

#define BUTTON_1    0
#define BUTTON_2    1
#define BUTTON_3    2
#define BUTTON_4    3

#define NUM_BUTTONS     4


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


/* FNS PROTOTYPES */

void Buttons_Init(Button_t *buttons);
int8_t Buttons_Update(Button_t *buttons);

void Faders_Init(Fader_t *faders);
int8_t Faders_Update(Fader_t *faders); 


void Encoders_Init(Encoder_t *encoder);


void UI_Init(Device_t *device);
void UI_Poll(Device_t *device);
