/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ceil, floor, fmod
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH


double ceil(double x) /* TODO testing */
{
	s64 tmp;

	tmp = (s64)x;

	if (tmp != (s64)x)
		++tmp;

	return (double)tmp;
}


double floor(double x) /* TODO testing */
{
	s64 tmp;

	tmp = (s64)x;

	return (double)tmp;
}


double fmod(double numer, double denom) /* TODO testing */
{
	double result;
	s64 tquot;

	if (denom == 0)
		return 0;

	tquot = numer / denom;
	result = tquot * denom;

	return numer - result;
}
