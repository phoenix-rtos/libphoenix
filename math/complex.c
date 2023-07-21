/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * mathematical functions on complex numbers
 *
 * Copyright 2023 Phoenix Systems
 * Author: Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <math.h>
#include <complex.h>


double cabs(double complex z)
{
	/* FIXME: due to missing hypot() implementation, temporarily sqrt() is used */
	return sqrt((creal(z) * creal(z)) + (cimag(z) * cimag(z)));
}


float cabsf(float complex z)
{
	/* FIXME: due to missing hypot() implementation, temporarily sqrtf() is used */
	return sqrtf((crealf(z) * crealf(z)) + (cimagf(z) * cimagf(z)));
}


double carg(double complex z)
{
	return atan2(cimag(z), creal(z));
}


float cargf(float complex z)
{
	return atan2f(cimagf(z), crealf(z));
}


double complex cexp(double complex z)
{
	double re = creal(z);
	double im = cimag(z);
	double radius = exp(re);
	return radius * cos(im) + radius * sin(im) * 1.0i;
}


float complex cexpf(float complex z)
{
	float re = crealf(z);
	float im = cimagf(z);
	float radius = expf(re);
	return radius * cosf(im) + radius * sinf(im) * 1.0fi;
}
