/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv8m)
 *
 * Copyright 2017, 2024, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
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
