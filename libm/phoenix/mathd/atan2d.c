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


double atan2(double y, double x)
{
	if ((isnan(y) != 0) || (isnan(x) != 0)) {
		return NAN;
	}

	if (y == 0.0) {
		if (x < 0.0 || signbit(x) != 0) {
			// x is negative or -0.0
			return (signbit(y) != 0) ? -M_PI : M_PI;
		}
		else {
			// x is positive or +0.0
			return y;
		}
	}

	if ((isinf(y) != 0) && (isinf(x) != 0)) {
		int sy = signbit(y);
		int sx = signbit(x);

		if ((sy == 0) && (sx != 0)) {
			return M_PI_4 * 3;
		}
		else if ((sy != 0) && (sx != 0)) {
			return -M_PI_4 * 3;
		}
		else if ((sy == 0) && (sx == 0)) {
			return M_PI_4;
		}
		else if ((sy != 0) && (sx == 0)) {
			return -M_PI_4;
		}
	}

	if (x > 0.0) {
		return atan(y / x);
	}
	else if (x < 0.0) {
		if (y >= 0.0) {
			return atan(y / x) + M_PI;
		}

		return atan(y / x) - M_PI;
	}

	if (y > 0.0) {
		return M_PI_2;
	}
	else if (y < 0.0) {
		return -M_PI_2;
	}

	return 0.0;
}
