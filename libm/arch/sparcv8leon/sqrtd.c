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


double sqrt(double x)
{
	/* clang-format off */
	__asm__ volatile ("fsqrtd %0, %0" : "+f"(x));
	/* clang-format on */

	return x;
}
