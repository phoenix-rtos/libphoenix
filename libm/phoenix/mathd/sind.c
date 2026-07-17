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

/* Calculates value of sine using Maclaurin series via Horner's method. */
double sin(double x)
{
	double res, xpow;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI) {
		x -= 2 * M_PI;
	}
	else if (x < -M_PI) {
		x += 2 * M_PI;
	}

	if (x > M_PI_2) {
		return cos(x - M_PI_2);
	}
	else if (x < -M_PI_2) {
		return -cos(x + M_PI_2);
	}

	xpow = x * x;

	res = 0x1.71b8ef6dcf572p-66;              /* 1/21! */
	res = res * xpow - 0x1.2f49b46814157p-57; /* 1/19! */
	res = res * xpow + 0x1.952c77030ad4ap-49; /* 1/17! */
	res = res * xpow - 0x1.ae7f3e733b81fp-41; /* 1/15! */
	res = res * xpow + 0x1.6124613a86d09p-33; /* 1/13! */
	res = res * xpow - 0x1.ae64567f544e4p-26; /* 1/11! */
	res = res * xpow + 0x1.71de3a556c734p-19; /* 1/9!  */
	res = res * xpow - 0x1.a01a01a01a01ap-13; /* 1/7!  */
	res = res * xpow + 0x1.1111111111111p-7;  /* 1/5!  */
	res = res * xpow - 0x1.5555555555555p-3;  /* 1/3!  */
	res = res * xpow + 0x1.0p0;               /* 1/1!  */

	return res * x;
}
