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


double log2(double x)
{
	return (log(x) / M_LN2);
}
