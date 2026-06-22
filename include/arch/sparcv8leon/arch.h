/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/sparcv8leon)
 *
 * Copyright 2022, 2026 Phoenix Systems
 * Author: Lukasz Leczkowski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_SPARCV8LEON_ARCH_H_
#define _LIBPHOENIX_ARCH_SPARCV8LEON_ARCH_H_

#define __ARCH_STDINT <arch/sparcv8leon/stdint.h>
#define __ARCH_LIMITS <arch/sparcv8leon/limits.h>

#define __MEMCPY
#define __MEMCMP
#define __MEMSET
#define __STRLEN
#define __STRCMP
#define __STRNCMP
#define __STRCPY
#define __STRNCPY

#ifndef _SOFT_FLOAT
#define __IEEE754_SQRT

static inline double __ieee754_sqrt(double x)
{
	/* clang-format off */
	__asm__ volatile ("fsqrtd %0, %0" : "+f"(x));
	/* clang-format on */

	return x;
}


#define __IEEE754_SQRTF
static inline float __ieee754_sqrtf(float x)
{
	/* clang-format off */
	__asm__ volatile ("fsqrts %0, %0" : "+f"(x));
	/* clang-format on */

	return x;
}

#endif

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
