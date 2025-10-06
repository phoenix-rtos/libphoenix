/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float one = 1.0f;

float sinhf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float t, w, h;
    int32_t ix, jx;

    GET_FLOAT_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (!FLT_UWORD_IS_FINITE(ix)) {
        return x + x;
    }

    h = 0.5f;

    if (jx < 0) {
        h = -h;
    }

    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x41b00000) {        /* |x|<22 */
        if (ix < 0x31800000) {    /* |x|<2**-28 */
            if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexactf(x);
            }
        }

        t = expm1f(fabsf(x));

        if (ix < 0x3f800000) {
            return h * (2.0f * t - t * t / (t + one));
        }

        return h * (t + t / (t + one));
    }

    /* |x| in [22, log(maxfloat)] return 0.5*exp(|x|) */
    if (ix <= FLT_UWORD_LOG_MAX) {
        return h * expf(fabsf(x));
    }

    /* |x| in [log(maxfloat), overflowthresold] */
    if (ix <= FLT_UWORD_LOG_2MAX) {
        w = expf(0.5f * fabsf(x));
        t = h * w;
        return t * w;
    }

    /* |x| > overflowthresold, sinh(x) overflow */
    return __raise_overflowf(x);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double sinh(double x)
{
    return (double) sinhf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
