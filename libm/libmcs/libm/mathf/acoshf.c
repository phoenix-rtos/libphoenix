/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float
one    = 1.0f,
ln2    = 6.9314718246e-01f;  /* 0x3f317218 */

float acoshf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float t;
    int32_t hx;
    GET_FLOAT_WORD(hx, x);

    if (hx < 0x3f800000) {     /* x < 1 */
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalidf();
        }
    } else if (hx >= 0x4d800000) {  /* x > 2**28 */
        if (!FLT_UWORD_IS_FINITE(hx)) {   /* x is +inf or NaN */
            return x + x;
        } else {
            return logf(x) + ln2;    /* acosh(huge)=log(2x) */
        }
    } else if (hx == 0x3f800000) {
        return 0.0f;            /* acosh(1) = 0 */
    } else if (hx > 0x40000000) {    /* 2**28 > x > 2 */
        t = x * x;
        return logf(2.0f * x - one / (x + sqrtf(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1pf(t + sqrtf(2.0f * t + t * t));
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double acosh(double x)
{
    return (double) acoshf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
