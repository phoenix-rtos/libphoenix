/*
 * Phoenix-RTOS
 *
 * libphoenix/libm
 *
 * Architecture dependent part (arch/aarch64)
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
 
#include <math.h>


float fabsf(float x)
{
	if (isnan(x)) {
		return x;
	}
	if (0 < x) {
		return x;
	}

	__asm__("fabs %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
