/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part
 *
 * Copyright 2017, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Lach
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>


double sqrt(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x < 0.0) {
		errno = EDOM;
		return NAN;
	}

	if ((x == 0.0) || (x == INFINITY)) {
		return x;
	}

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
