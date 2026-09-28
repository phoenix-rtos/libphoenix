/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * CPU clock internal definitions
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _LIBPHOENIX_COMMON_CPUCLOCK_H_
#define _LIBPHOENIX_COMMON_CPUCLOCK_H_


#include <limits.h>
#include <time.h>


#define CPUCLOCK_SHIFT (3U)
#define CPUCLOCK_MASK  (7U)

#define CPUCLOCK_ID_THREAD(tid)  ((((unsigned int)(tid)) << CPUCLOCK_SHIFT) | CLOCK_THREAD_CPUTIME_ID)
#define CPUCLOCK_ID_PROCESS(pid) ((((unsigned int)(pid)) << CPUCLOCK_SHIFT) | CLOCK_PROCESS_CPUTIME_ID)

#define CPUCLOCK_ID_VALUE(clock) (((unsigned int)(clock)) >> CPUCLOCK_SHIFT)

#define CPUCLOCK_IS_THREAD(clock)  ((((unsigned int)(clock)) & CPUCLOCK_MASK) == CLOCK_THREAD_CPUTIME_ID)
#define CPUCLOCK_IS_PROCESS(clock) ((((unsigned int)(clock)) & CPUCLOCK_MASK) == CLOCK_PROCESS_CPUTIME_ID)

#define CPUCLOCK_IS_DYNAMIC(clock) ((((unsigned int)(clock)) >> CPUCLOCK_SHIFT) != 0U)

#define CPUCLOCK_ID_MAX     (((unsigned int)INT_MAX) >> CPUCLOCK_SHIFT)
#define CPUCLOCK_ID_FITS(v) (((v) >= 0) && ((v) <= CPUCLOCK_ID_MAX))


#endif /* _LIBPHOENIX_COMMON_CPUCLOCK_H_ */
