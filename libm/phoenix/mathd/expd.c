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
#include "common.h"


/* Uses quick powering and Maclaurin series to calculate value of e^x */
double exp(double x)
{
	double res, resi, powx, e, factorial;
	int i;

	if (isnan(x) != 0) {
		return NAN;
	}

	/* Values of x greater than 709.79 will cause overflow, returning INFINITY */
	if (x > 709.79) {
		errno = ERANGE;
		return HUGE_VAL;
	}

	/* Get floor of exponent */
	x = modf(x, &e);

	/* Calculate most of the result */
	resi = quickPow(M_E, (int)e);

	/* Calculate rest of the result using Maclaurin series */
	factorial = 1.0;
	powx = x;
	res = 1.0;

	for (i = 2; i < 13; ++i) {
		if (powx == 0.0) {
			break;
		}
		res += powx / factorial;
		factorial *= i;
		powx *= x;
	}

	return (res * resi);
}
