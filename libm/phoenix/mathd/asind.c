/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, tan, acos, asin, atan
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Aleksander Kaminski, Jakub Smolaga
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include <math.h>


/* Calculates value of arc sine using asin(x) = pi/2 - acos(x) relationship. */
double asin(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	return M_PI_2 - acos(x);
}
