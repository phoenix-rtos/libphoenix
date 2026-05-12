/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/futex.h
 *
 * Copyright 2025 Phoenix Systems
 * Author: kamil kowalczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_FUTEX_H_
#define _LIBPHOENIX_SYS_FUTEX_H_

#include <time.h>
#include <stdint.h>

#define FUTEX_WAKEUP_ALL ((uint32_t) - 1)


extern int phFutexWait(_Atomic(uint32_t) *address, uint32_t value, time_t timeout, int clockType);


extern int phFutexWakeup(_Atomic(uint32_t) *address, uint32_t n_threads);


#endif
