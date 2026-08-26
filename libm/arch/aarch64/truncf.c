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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>


float truncf(float x)
{
	__asm__ volatile("frintz %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
