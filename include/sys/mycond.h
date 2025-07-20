#ifndef _LIBPHOENIX_MYCOND_H_
#define _LIBPHOENIX_MYCOND_H_

#include <string.h>
#include <stdint.h>
#include <sys/futex.h>
#include <sys/mymutex.h>

typedef struct {
	_Atomic(uint32_t) value;
} mycond_t;

#define MC_INIT(mc) \
	do { \
		memset((mc), 0, sizeof(*(mc))); \
	} while (0)

void mc_wait(mycond_t *mc, mymutex_t *mm);
void _mc_signal_n(mycond_t *mc, uint32_t n);

#define mc_signal(mc)     _mc_signal_n((mc), 1)
#define mc_signal_all(mc) _mc_signal_n((mc), FUTEX_WAKEUP_ALL)

#endif
