/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * hyberbolic functions
 *
 * Copyright 2018 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <math.h>
#include <float.h>
#include <errno.h>


double cosh(double x)
{
	double y;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		return INFINITY;
	}

	/* Make sure exp(x) is not infinity */
	if (x < 709.78) {
		return ((exp(x) + exp(-x)) / 2.0);
	}
	else {
		y = cosh(x / 2.0);
		return ((2.0 * y * y) - 1.0);
	}
}


double sinh(double x)
{
	double y, f;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0 || (x == 0.0)) {
		return x;
	}

	/* Ensure exp(x) does not overflow. Values of x less than 709.78 are guaranteed to be safe. */
	if (x < 709.78) {
		return ((exp(x) - exp(-x)) * 0.5);
	}
	else {
		y = sinh(x / 3.0);
		f = (3.0 * y) + (4.0 * y * y * y);

		/* ia32 use 80-bit extended precision registers so isinf() may not be true */
		if ((isinf(f) != 0) || (fabs(f) > DBL_MAX)) {
			errno = ERANGE;
			return (f > 0.0) ? HUGE_VAL : -HUGE_VAL;
		}

		return f;
	}
}


double tanh(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x == INFINITY) {
		return 1.0;
	}
	else if (x == -INFINITY) {
		return -1.0;
	}

	/* cosh is never equal to zero */
	return (sinh(x) / cosh(x));
}
