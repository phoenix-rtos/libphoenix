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


double log(double x)
{
	double tmp, pow, res;
	conv_t *conv = (conv_t *)&tmp;
	int exp = 0, i;

	if (isnan(x) != 0) {
		return NAN;
	}
	else if (x < 0.0) {
		errno = EDOM;
		return NAN;
	}
	else if (x == 0.0) {
		errno = ERANGE;
		return -HUGE_VAL;
	}
	else if (x == 1.0) {
		return 0.0;
	}
	else if (isinf(x) != 0) {
		return x;
	}

	tmp = x;

	exp = conv->i.exponent - 1022;

	if (conv->i.exponent == 0) {
		normalizeSub(&tmp, &exp);
	}

	conv->i.exponent = 1022;

	tmp = (tmp - 1.0) / (tmp + 1.0);

	for (i = 1, res = 0.0, pow = tmp * tmp; i < 16; ++i) {
		res += tmp / ((2 * i) - 1);
		tmp *= pow;
	}

	return ((2.0 * res) + (exp / M_LOG2E));
}
