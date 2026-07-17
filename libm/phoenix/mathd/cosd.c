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
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include <math.h>
#include <errno.h>


/* Calculates value of cosine using Maclaurin series via Horner's method. */
double cos(double x)
{
	double res, xpow;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI) {
		x -= 2.0 * M_PI;
	}
	else if (x < -M_PI) {
		x += 2.0 * M_PI;
	}

	if (x > M_PI_2) {
		return -sin(x - M_PI_2);
	}
	else if (x < -M_PI_2) {
		return sin(x + M_PI_2);
	}

	xpow = x * x;

	res = 0x1.e542ba4020225p-62;              /* 1/20! */
	res = res * xpow - 0x1.6827863b97d97p-53; /* 1/18! */
	res = res * xpow + 0x1.ae7f3e733b81fp-45; /* 1/16! */
	res = res * xpow - 0x1.93974a8c07c9dp-37; /* 1/14! */
	res = res * xpow + 0x1.1eed8eff8d898p-29; /* 1/12! */
	res = res * xpow - 0x1.27e4fb7789f5cp-22; /* 1/10! */
	res = res * xpow + 0x1.a01a01a01a01ap-16; /* 1/8!  */
	res = res * xpow - 0x1.6c16c16c16c17p-10; /* 1/6!  */
	res = res * xpow + 0x1.5555555555555p-5;  /* 1/4!  */
	res = res * xpow - 0x1p-1;                /* 1/2!  */
	res = res * xpow + 0x1p0;                 /* 1/0!  */

	return res;
}
