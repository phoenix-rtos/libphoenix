/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/errorfunctionf.h"

static const float
half =  5.0000000000e-01f, /* 0x3F000000 */
two  =  2.0000000000e+00f, /* 0x40000000 */
erx  =  8.42697144e-01f;   /* 0x3f57bb00 */

float erfcf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, ix;
    float R, S, P, Q, s, y, z, r;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (!FLT_UWORD_IS_FINITE(ix)) {
        if (isnan(x)) {         /* erfc(nan) = nan */
            return x + x;
        } else if (hx > 0) {    /* erfc(+inf) = 0 */
            return 0.0f;
        } else {                /* erfc(-inf) = 2 */
            return two;
        }
    }

    if (ix < 0x3f580000) {       /* |x|<0.84375 */
        if (ix < 0x33800000) {   /* |x|<2**-24 */
            return __raise_inexactf(one);
        }

        y = __erff_y(x);

        if (hx < 0x3e800000) {     /* x<1/4 */
            return one - (x + x * y);
        } else {
            r = x * y;
            r += (x - half);
            return half - r ;
        }
    }

    if (ix < 0x3fa00000) {       /* 0.84375 <= |x| < 1.25 */
        s = fabsf(x) - one;
        P = __erff_P(s);
        Q = __erff_Q(s);

        if (hx >= 0) {
            z  = one - erx;
            return z - P / Q;
        } else {
            z = erx + P / Q;
            return one + z;
        }
    }

    if (ix < 0x41220000) {        /* |x|<10.125 */
        x = fabsf(x);
        s = one / (x * x);

        if (ix < 0x4036DB6D) {  /* |x| < 1/.35 ~ 2.857143*/
            R = __erff_Ra(s);
            S = __erff_Sa(s);
        } else {            /* |x| >= 1/.35 ~ 2.857143 */
            if (hx < 0 && ix >= 0x40c00000) {
                return __raise_inexactf(two);    /* x < -6 */
            }

            R = __erff_Rb(s);
            S = __erff_Sb(s);
        }

        GET_FLOAT_WORD(ix, x);
        SET_FLOAT_WORD(z, ix & 0xffffc000U);
        r  =  expf(-z * z - 0.5625f) * expf((z - x) * (z + x) + R / S);

        if (hx > 0) {
            return r / x;
        } else {
            return two - r / x;
        }
    } else {
        if (hx > 0) {
            return __raise_underflowf(0.0f);
        } else {
            return __raise_inexactf(two);
        }
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double erfc(double x)
{
    return (double) erfcf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
