/*
 * Phoenix-RTOS
 *
 * libphoenix/libm
 *
 * Architecture dependent part (RISCV64)
 *
 * Copyright 2018, 2026 Phoenix Systems
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

	/* clang-format off */
	__asm__ volatile ("fsqrt.d %0, %1" : "=f"(x) : "f"(x));
	/* clang-format on */

	return x;
}
