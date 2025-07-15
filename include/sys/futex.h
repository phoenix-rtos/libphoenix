#ifndef _LIBPHOENIX_SYS_FUTEX_H_
#define _LIBPHOENIX_SYS_FUTEX_H_

#include <time.h>

int phFutexWait(unsigned int *address, unsigned int value, time_t timeout);
int phFutexWakeup(unsigned int *address, unsigned int n_threads);

#endif
