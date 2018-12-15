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
 * %LICENSE%
 */

#include <math.h>


double cosh(double x)
{
	if (x == INFINITY || x == -INFINITY)
		return INFINITY;

	return (exp(x) + exp(-x)) / 2;
}


double sinh(double x)
{
	if (x == INFINITY || x == -INFINITY)
		return x;

	return (exp(x) - exp(-x)) / 2;
}


double tanh(double x)
{
	/* cosh is never equal to zero */

	return sinh(x) / cosh(x);
}
