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


float roundf(float x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	__asm__ volatile("frinta %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
