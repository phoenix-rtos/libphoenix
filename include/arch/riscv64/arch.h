/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (RISCV64)
 *
 * Copyright 2018, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_RISCV64_ARCH_H_
#define _LIBPHOENIX_ARCH_RISCV64_ARCH_H_

#define __ARCH_STDINT <arch/riscv64/stdint.h>
#define __ARCH_LIMITS <arch/riscv64/limits.h>
#define __ARCH_SYS_IO <arch/riscv64/io.h>

#define __MEMCPY
#define __MEMSET

#ifndef __SOFTFP__
#define __IEEE754_SQRT

static inline double __ieee754_sqrt(double x)
{
	/* clang-format off */
	__asm__ volatile ("fsqrt.d %0, %1" : "=f"(x) : "f"(x));
	/* clang-format on */

	return x;
}


#define __IEEE754_SQRTF
static inline float __ieee754_sqrtf(float x)
{
	/* clang-format off */
	__asm__ volatile ("fsqrt.s %0, %1" : "=f"(x) : "f"(x));
	/* clang-format on */

	return x;
}

#endif

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
