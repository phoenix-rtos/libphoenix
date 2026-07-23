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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>


double tan(double x)
{
	double c;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	c = cos(x);

	if (c != 0.0) {
		return (sin(x) / c);
	}
	else {
		return 0.0;
	}
}
