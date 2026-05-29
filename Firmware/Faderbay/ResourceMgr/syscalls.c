/* =========================================================
 * syscalls.c
 * Resource Manager layer — Newlib syscall stubs for bare-metal
 *
 * NOTE: _exit, _kill, _getpid, _read, _write, _close and friends
 * are already provided by Core/Src/syscalls.c (CubeMX-generated).
 * _sbrk is in Core/Src/sysmem.c. Only add stubs here that are
 * genuinely absent from Core/.
 * ========================================================= */

#include <sys/time.h>

/* No real-time clock on bare metal; satisfies NanoLog timestamp calls.
 * Core/Src/syscalls.c includes <sys/time.h> but does not define this. */
int _gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    if (tv) { tv->tv_sec = 0; tv->tv_usec = 0; }
    return 0;
}
