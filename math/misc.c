/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fabs
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH


double fabs(double x)
{
	return x < 0.0 ? -x : x;
}
