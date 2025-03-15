## librerias
midi? Mirar librerias o hacer pequeño driver

## defines


## structs

faderbay

fader

button

knob

screen




Que hacer
=======


- Leer faders (Polling mux)
- Guardar en eeprom
- Mostrar buffer por tft
- Polling botones + encoder
- Enviar midi
- Manejar Configuraciones del menu


```c

int main(void) {
    HAL_Init();  
    SystemClock_Config();  
    MX_GPIO_Init();  
    MX_ADC1_Init();  
    MX_SPI1_Init();  
    MX_UART1_Init();  

    UI_Init();
    OLED_Init();
    MIDIHandler_Init();
    MemoryManager_Init();

    while (1) {
        UI_Poll();
        OLED_Update();
        MIDIHandler_Process();
    }
}
```
## Archivos

- `main.c`          - Runs the program
- `main.h`          - Pinout and other defines
- `config.h`        - Configurations (debug modes, etc)
- `device.c`        - Device APIs (Init device, manage other APIs) Maybe do it on main.c?
- `UI.c`            - UI general API (Polling, Init, etc)
- `UI.h`            - Fns prototypes
- `fader.c`         - Fader API (Read, MUX, etc)
- `fader.h`         - Fns prototypes, struct, defines
- `button.c`        - Button API (Read, init, etc)
- `button.h`        - Fns prototypes, struct, defines
- `encoder.c`       - Encoder API (Read, init, etc)
- `encoder.h`       - Fns protoypes, struct, defines
- `oled.c`          - OLED API (write, init, etc)
- `oled.h`          - Fns prototypes, struct, defines
- `midi.c`          - MIDI API (Init, send CC, USB Midi device, change CC, change channel, etc). Use a library?
- `midi.h`          - Fns prototypes, structs, defines, etc
- `eeprom.c`        - EEPROM API (save, read, init, etc)
- `eeprom.h`        - Fns prototypes, struct, defines
- `menu_config.c`   - Fns for device configurations (Save configurations, open configurations, turn on/off CV outs, etc)
- `menu_config.h`   - Fns prototypes, etc
- `usb.c`           - To configure as a USB device (see how)


## Route
1. start stm32cubeide project
2. UI interfaces (faders, buttons, encoder)
3. code device struct
4. poll UI to change device settings
5. EEPROM to save configs
6. MIDI Interface 
7. MIDI Device (USB Device)
8. OLED