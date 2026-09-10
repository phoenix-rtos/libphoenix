/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/sparcv8leon)
 *
 * Copyright 2022, 2026 Phoenix Systems
 * Author: Lukasz Leczkowski, Michal Lach
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>


float sqrtf(float x)
{
	if (x < 0.0f) {
		errno = EDOM;
		return NAN;
	}
	/* clang-format off */
	__asm__ volatile ("fsqrts %0, %0" : "+f"(x));
	/* clang-format on */

	return x;
}
