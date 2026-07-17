/*
 * Phoenix-RTOS
 *
 * libphoenix/libm
 *
 * Architecture dependent part (arch/armv7r)
 *
 * Copyright 2017, 2024, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Lukasz Leczkowski, Michal Lach
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

	__asm__ volatile("vsqrt.f64 %P0, %P1" : "=w"(x) : "w"(x));
	return x;
}
