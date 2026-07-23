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


/* Calculates value of arc cosine using secant method */
double acos(double x)
{
	double xa = 0, xb = M_PI, ya, yb, t;
	int i;

	if (isnan(x) != 0) {
		return NAN;
	}

	if ((x > 1.0) || (x < -1.0)) {
		errno = EDOM;
		return NAN;
	}

	for (i = 0; i < 16; ++i) {
		ya = cos(xa) - x;
		yb = cos(xb) - x;

		t = ya - yb;

		if (t == 0.0) {
			break;
		}

		t = ((ya * xb) - (yb * xa)) / t;
		xa = xb;
		xb = t;
	}

	return xb;
}
