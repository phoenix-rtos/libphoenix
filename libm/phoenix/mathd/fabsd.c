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


double fabs(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	conv_t *conv = (conv_t *)&x;
	conv->i.sign = 0;

	return x;
}
