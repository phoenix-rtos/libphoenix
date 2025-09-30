/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

/*
 * copysignf(float x, float y)
 * copysignf(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include <math.h>
#include "../common/tools.h"

float copysignf(float x, float y)
{
    uint32_t ix, iy;
    GET_FLOAT_WORD(ix, x);
    GET_FLOAT_WORD(iy, y);
    SET_FLOAT_WORD(x, (ix & 0x7fffffffU) | (iy & 0x80000000U));
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double copysign(double x, double y)
{
    return (double) copysignf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
