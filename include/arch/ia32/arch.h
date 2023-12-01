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
	unsigned short newcw, savecw;
	unsigned short register reg;

	/* clang-format off */
	__asm__ volatile (
		"fstcw %w[savecw]\n\t"        /* save control word */
		"mov %w[savecw], %w[reg]\n\t" /* reg <- (memptr) */
		"and $0xf0ff, %w[reg]\n\t"    /* inherit exception mask and infinity */
		"or $0x200, %w[reg]\n\t"      /* set double precision, round to nearest-even */
		"mov %w[reg], %w[newcw]\n\t"  /* (memptr) <- reg */
		"fldcw %w[newcw]\n\t"         /* and apply (memptr) new control word */
		"fsqrt\n\t"                   /* calculate double precision sqrt */
		"fldcw %w[savecw]"            /* restore control word */
		: "+t" (x), [reg] "=&r" (reg)
		: [newcw] "m" (newcw), [savecw] "m" (savecw)
		: "cc", "memory");
	/* clang-format on */

	return x;
}

#endif

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
