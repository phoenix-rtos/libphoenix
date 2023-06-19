/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/arm-imx)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARM_IMX_ARCH_H_
#define _LIBPHOENIX_ARCH_ARM_IMX_ARCH_H_

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __ARCH_STDINT <arch/arm-imx/stdint.h>
#define __ARCH_LIMITS <arch/arm-imx/limits.h>
#define __ARCH_SYS_TYPES <arch/arm-imx/types.h>

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

#if (__ARM_PCS_VFP || (__VFP_FP__ && !__SOFTFP__)) && (__ARM_FP & 8)
#define __IEEE754_SQRT

static inline double __ieee754_sqrt(double x)
{
	__asm__ volatile ("vsqrt.f64 %P0, %P1" : "=w"(x) : "w"(x));

	return x;
}
#endif

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
