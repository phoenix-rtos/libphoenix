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
#include <errno.h>


double fmod(double number, double denom)
{
	double result, tquot;

	if (isnan(number) != 0 || isnan(denom) != 0) {
		return NAN;
	}

	if ((denom == 0.0) || (isinf(number) != 0)) {
		errno = EDOM;
		return NAN;
	}

	if (((number == 0.0) && (denom != 0.0)) ||
			((isinf(number) == 0) && (isinf(denom) != 0))) {
		return number;
	}

	modf(number / denom, &tquot);
	result = tquot * denom;

	return number - result;
}
