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


double ldexp(double x, int exp)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	conv_t *conv = (conv_t *)&x;
	int exponent = 0;

	if (conv->i.exponent == 0) {
		normalizeSub(&x, &exponent);
	}

	exponent += conv->i.exponent + exp;

	if (exponent > 2046) {
		errno = ERANGE;
		return conv->i.sign ? -HUGE_VAL : HUGE_VAL;
	}

	/* If result is subnormal */
	if (exponent < 0) {
		createSub(&x, exponent);
		conv->i.exponent = 0;
	}
	else {
		conv->i.exponent = exponent;
	}

	return x;
}
