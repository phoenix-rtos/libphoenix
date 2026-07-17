/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pow, sqrt
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


double sqrt(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x < 0.0) {
		errno = EDOM;
		return NAN;
	}

	if ((x == 0.0) || (x == INFINITY)) {
		return x;
	}

	/* Use reciprocal square root method: */
	double xn = 1.0 / x;

	/* IEEE-754 compliant: */
	conv_t *init = (conv_t *)&xn;

	/* +Infinity : */
	if ((init->i.mantisa == 0) && (init->i.exponent == 0x7FF)) {
		return x;
	}

	/* Subnormals: */
	if (init->i.exponent == 0) {
		init->i.exponent = 0x1;
	}

	/* Initial guess: */
	init->i.mantisa = (init->i.mantisa >> 1);

	if (init->i.exponent & 0x1) {
		init->i.exponent = (init->i.exponent >> 1) + 0x200;
	}
	else {
		init->i.exponent = (init->i.exponent >> 1) + 0x1FF;
		init->i.mantisa |= (0x1ull << 51);
	}

	/* Reciprocal sqare root iters (avoiding division): */
	for (int i = 0; i < 4; ++i) {
		xn = xn * (1.5 - (0.5 * x * xn * xn));
	}

	return (xn * x);
}
