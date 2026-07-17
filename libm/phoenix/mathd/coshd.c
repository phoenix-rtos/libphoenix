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
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include <math.h>


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
