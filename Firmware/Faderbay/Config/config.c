/* =========================================================
 * config.c
 * Customizable layer — Default device configuration
 * ========================================================= */

#include "config.h"
#include <sys/time.h>

// ========================= PRIVATE DEFINES =======================

#define DEFAULT_FADER_CONFIG(cc) \
    { .midi_channel = 1, .midi_cc = (cc), .mode = FADER_MODE_MIDI }

// =========================== PUBLIC DATA =========================

DeviceConfig_t g_config = {
    .faders = {
        DEFAULT_FADER_CONFIG(7),
        DEFAULT_FADER_CONFIG(8),
        DEFAULT_FADER_CONFIG(9),
        DEFAULT_FADER_CONFIG(10),
        DEFAULT_FADER_CONFIG(11),
        DEFAULT_FADER_CONFIG(12),
        DEFAULT_FADER_CONFIG(13),
        DEFAULT_FADER_CONFIG(14),
        DEFAULT_FADER_CONFIG(15),
        DEFAULT_FADER_CONFIG(16),
        DEFAULT_FADER_CONFIG(17),
        DEFAULT_FADER_CONFIG(18),
        DEFAULT_FADER_CONFIG(19),
        DEFAULT_FADER_CONFIG(20),
        DEFAULT_FADER_CONFIG(21),
        DEFAULT_FADER_CONFIG(22),
    },
    .active_preset = 0,
};

// Newlib syscall stub — bare metal has no real-time clock
int _gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    if (tv) { tv->tv_sec = 0; tv->tv_usec = 0; }
    return 0;
}
