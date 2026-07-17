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


/* Uses log10(x) = ln(x) / ln(10) identity */
double log10(double x)
{
	return (log(x) / M_LN10);
}
