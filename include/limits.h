/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * limits.h
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIMITS_H_
#define _LIMITS_H_

#include <arch.h>


#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4
#define PTHREAD_DESTRUCTOR_ITERATIONS       _POSIX_THREAD_DESTRUCTOR_ITERATIONS

#ifdef __ARCH_LIMITS
#include __ARCH_LIMITS
#else
#error "Required header limits.h is not defined for current architecture!"
#endif

#include <sys/syslimits.h>
#include <phoenix/limits.h>

#endif
