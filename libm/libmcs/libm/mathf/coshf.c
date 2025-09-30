/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float one = 1.0f, half = 0.5f;

float coshf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float t, w;
    int32_t ix;

    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;

    /* x is INF or NaN */
    if (!FLT_UWORD_IS_FINITE(ix)) {
        return x * x;
    }

    /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
    if (ix < 0x3eb17218) {
        t = expm1f(fabsf(x));
        w = one + t;

        if (ix < 0x24000000) {
            return w;    /* cosh(tiny) = 1 */
        }

        return one + (t * t) / (w + w);
    }

    /* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
    if (ix < 0x41b00000) {
        t = expf(fabsf(x));
        return half * t + half / t;
    }

    /* |x| in [22, log(maxfloat)] return half*exp(|x|) */
    if (ix <= FLT_UWORD_LOG_MAX) {
        return half * expf(fabsf(x));
    }

    /* |x| in [log(maxfloat), overflowthresold] */
    if (ix <= FLT_UWORD_LOG_2MAX) {
        w = expf(half * fabsf(x));
        t = half * w;
        return t * w;
    }

    /* |x| > overflowthresold, cosh(x) overflow */
    return __raise_overflowf(1.0f);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double cosh(double x)
{
    return (double) coshf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
