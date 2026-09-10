/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7m)
 *
 * Copyright 2017, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>


float sqrtf(float x)
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

	__asm__ volatile("vsqrt.f32 %0, %1" : "=t"(x) : "t"(x));
	return x;
}
