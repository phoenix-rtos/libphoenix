/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * exp, frexp, ldexp, log, log10, modf, ceil, floor, fmod, fabs
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>


double round(double x)
{
	double ret, frac;

	if (isnan(x) != 0) {
		return NAN;
	}

	frac = modf(x, &ret);

	if (frac >= 0.5) {
		ret += 1.0;
	}
	else if (frac <= -0.5) {
		ret -= 1.0;
	}

	return ret;
}
