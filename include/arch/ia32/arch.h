/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_ARCH_H_
#define _LIBPHOENIX_ARCH_IA32_ARCH_H_

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __ARCH_STDINT <arch/ia32/stdint.h>
#define __ARCH_LIMITS <arch/ia32/limits.h>
#define __ARCH_SYS_TYPES <arch/ia32/types.h>
#define __ARCH_SYS_IO <arch/ia32/io.h>

#define __MEMCPY
#define __MEMSET

#if (!__SOFTFP__)
#define __IEEE754_SQRT

static inline double __ieee754_sqrt(double x)
{
	double result;

	__asm__ volatile ("fldl %1\n\t" /* put value */
		"fsqrt\n\t"                 /* calc sqrt */
		"fxtract\n\t"               /* extract exponent */
		"fisttp %0\n\t"             /* round the exponent */
		"fld %0\n\t"                /* load the rounded exponent */
		"fcomp\n\t"                 /* compare with the original exponent */
		"fstsw %%ax\n\t"            /* save FPU status to AX register */
		"sahf\n\t"                  /* copy status to CPU flags */
		"jp 1f\n\t"                 /* jump if the result was exact */
		"fsubrp\n"                  /* adjust mantissa down */
		"1:\n\t"
		"fstpl %0"                  /* get the result */
		: "=m"(result)
		: "m"(x));

	return result;
}

#endif

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
