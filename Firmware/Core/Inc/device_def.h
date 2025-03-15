#ifndef __DEVICE_DEF__
#define __DEVICE_DEF__

#include "app_config.h"


typedef enum {
    DEVICE_OK = 0,             // Éxito (valor positivo o 0)
    DEVICE_ERROR = -1,         // Error genérico
    DEVICE_INVALID_PARAM = -2, // Error por parámetro inválido
    DEVICE_UART_ERROR = -3,    // Error en UART
    DEVICE_UI_INIT_ERROR = -4,     // Error en I2C
    DEVICE_CLI_ERROR = -5,     // Error en SPI
    DEVICE_ADC_ERROR = -6,       // Operación con timeout
    DEVICE_NOT_READY = -7,     // Dispositivo no está listo
    DEVICE_MEMORY_ERROR = -8   // Error de memoria
} device_error_t;

const char* Device_GetErrorMessage(device_error_t error);

#define DEVICE_PROCESS(fn)                          \
do {                                               \
    device_error_t _status = (device_error_t)(fn);                 \
    if (_status != DEVICE_OK) {                    \
        printf("[ERROR]: %s\n", Device_GetErrorMessage(_status)); \
        /* return _status; */                            \
    }                                           \
} while (0)


#ifdef USE_CLI
    #define PRINT(msg) { cli.println(">> "); cli.println(msg); }
#else
    #include <stdio.h>
    #define PRINT(msg) printf("%s\n", msg)
#endif



#endif