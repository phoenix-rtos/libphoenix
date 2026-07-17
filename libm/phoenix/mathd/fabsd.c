/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
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
