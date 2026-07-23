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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>


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
