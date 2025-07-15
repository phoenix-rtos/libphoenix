#ifndef _LIBPHOENIX_SYS_FUTEX_H_
#define _LIBPHOENIX_SYS_FUTEX_H_

#include <time.h>
#include <stdint.h>

#define FUTEX_WAKEUP_ALL ((uint32_t) - 1)

int phFutexWait(_Atomic(uint32_t) *address, uint32_t value, time_t timeout);
int phFutexWakeup(_Atomic(uint32_t) *address, uint32_t n_threads);
int phFutexRequeue(_Atomic(uint32_t) *address1, _Atomic(uint32_t) *address2, uint32_t n_wakeup, uint32_t n_requeue);

#endif
