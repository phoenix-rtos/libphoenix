/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, tan, acos, asin, atan
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Aleksander Kaminski, Jakub Smolaga
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>


float atanf(float x)
{
	return (float)atan((double)x);
}
