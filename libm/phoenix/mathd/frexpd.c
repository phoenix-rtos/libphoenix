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
#include "common.h"


double frexp(double x, int *exp)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		return x;
	}

	if (x == 0.0) {
		return x;
	}

	conv_t *conv = (conv_t *)&x;
	*exp = 0;

	if (conv->i.exponent == 0) {
		normalizeSub(&x, exp);
	}

	*exp += conv->i.exponent - 1022;
	conv->i.exponent = 1022;

	return x;
}
