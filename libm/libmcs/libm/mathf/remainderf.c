/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float zero = 0.0f;

float remainderf(float x, float y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, hy;
    uint32_t sx;
    float y_half;

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);
    sx = hx & 0x80000000U;
    hy &= 0x7fffffff;
    hx &= 0x7fffffff;

    /* purge off exception values */
    if (FLT_UWORD_IS_NAN(hx) || FLT_UWORD_IS_NAN(hy)) {                 /* x or y is NaN */
        return x + y;
    } else if (FLT_UWORD_IS_ZERO(hy) || FLT_UWORD_IS_INFINITE(hx)) {    /* y is 0 or x is inf */
        return __raise_invalidf();
    } else {
        /* No action required */
    }

    if (hy <= FLT_UWORD_HALF_MAX) {
        x = fmodf(x, 2 * y);    /* now x < 2y */
    }

    if ((hx - hy) == 0) {
        return zero * x;
    }

    x  = fabsf(x);
    y  = fabsf(y);

    if (hy < 0x01000000) {
        if (x + x > y) {
            x -= y;

            if (x + x >= y) {
                x -= y;
            }
        }
    } else {
        y_half = 0.5f * y;

        if (x > y_half) {
            x -= y;

            if (x >= y_half) {
                x -= y;
            }
        }
    }

    GET_FLOAT_WORD(hx, x);
    SET_FLOAT_WORD(x, hx ^ sx);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double remainder(double x, double y)
{
    return (double) remainderf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
