#ifndef FADERBAY_TYPES_H
#define FADERBAY_TYPES_H

// ============================= INCLUDES ==============================

#include <stdint.h>

// ============================ PUBLIC DEFINES =========================

// ============================ PUBLIC TYPES ===========================

typedef enum {
    FB_OK                   = 0,             // Éxito (valor positivo o 0)
    FB_ERR_GENERIC          = 1,         // Error genérico
    FB_ERR_INVALID_PARAM    = 2,   // Error por parámetro inválido
    FB_ERR_UART             = 3,    // Error en UART
    FB_ERR_SPI              = 4,
    FB_ERR_ADC              = 5,       // Operación con timeout
    FB_ERR_NOT_READY        = 6,     // Dispositivo no está listo
    FB_ERR_MEMORY           = 7,   // Error de memoria
    FB_ERR_TIMER            = 8
} fb_err_t;

typedef enum {
    FADER_MODE_OFF          = 0,    /* Fader not activated */
    FADER_MODE_MIDI         = 1,    /* Fader MIDI mode only */
    FADER_MODE_MIDI_CV      = 2,    /* Fader MIDI and CV mode */
    FADER_MODE_CV           = 3     /* Fader CV mode only*/
} FaderMode_t;

typedef enum {
    UI_MODE_PERFORMANCE     = 0,
    UI_MODE_EDITING         = 1,
    UI_MODE_CONFIG          = 2
} AppState_t;

// ========================= PUBLIC FUNC. DECL. ========================

#endif /* FADERBAY_TYPES_H */
