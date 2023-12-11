/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7a)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV7A_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV7A_ARCH_H_

#define __ARCH_STDINT    <arch/armv7a/stdint.h>
#define __ARCH_LIMITS    <arch/armv7a/limits.h>
#define __ARCH_SYS_TYPES <arch/armv7a/types.h>

#define __MEMCPY
#define __MEMCMP
#define __MEMSET
#define __STRLEN
#define __STRNLEN
#define __STRCMP
#define __STRNCMP
#define __STRCPY
#define __STRNCPY
#define __MEMMOVE


#if __ARM_PCS_VFP || (__VFP_FP__ && !__SOFTFP__)
#if __ARM_FP & 8
#define __IEEE754_SQRT

static inline double __ieee754_sqrt(double x)
{
	/* clang-format off */
	__asm__ volatile ("vsqrt.f64 %P0, %P1" : "=w"(x) : "w"(x));
	/* clang-format on */

	return x;
}
#endif

#define __IEEE754_SQRTF

static inline float __ieee754_sqrtf(float x)
{
	/* clang-format off */
	__asm__ volatile ("vsqrt.f32 %0, %1" : "=t"(x) : "t"(x));
	/* clang-format on */

	return x;
}
#endif

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE  _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
