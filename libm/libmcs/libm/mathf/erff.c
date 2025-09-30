/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/errorfunctionf.h"

static const float
erx  =  8.42697144e-01f,   /* 0x3f57bb00 */
/*
 * In the domain [0, 2**-14], only the first term in the power series
 * expansion of erf(x) is used.  The magnitude of the first neglected
 * terms is less than 2**-42.
 */
efx  =  1.2837916613e-01f, /* 0x3e0375d4 */
efx8 =  1.0270333290e+00f; /* 0x3f8375d4 */

float erff(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, ix;
    float R, S, P, Q, s, z, r;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (!FLT_UWORD_IS_FINITE(ix)) {
        if (isnan(x)) {         /* erf(nan) = nan */
            return x + x;
        } else if (hx > 0) {    /* erf(+inf) = +1 */
            return 1.0f;
        } else {                /* erf(-inf) = -1 */
            return -1.0f;
        }
    }

    if (ix < 0x3f580000) {       /* |x|<0.84375 */
        if (ix < 0x38800000) {    /* |x|<2**-14 */
            if (ix < 0x04000000)
                /*avoid underflow */
            {
                return 0.125f * (8.0f * x + efx8 * x);
            }

            return x + efx * x;
        }

        return x + x * __erff_y(x);
    }

    if (ix < 0x3fa00000) {       /* 0.84375 <= |x| < 1.25 */
        s = fabsf(x) - one;
        P = __erff_P(s);
        Q = __erff_Q(s);

        if (hx >= 0) {
            return erx + P / Q;
        } else {
            return -erx - P / Q;
        }
    }

    if (ix >= 0x40800000) {        /* inf>|x|>=4 */
        if (hx >= 0) {
            return __raise_inexactf(one);
        } else {
            return -__raise_inexactf(one);
        }
    }

    x = fabsf(x);
    s = one / (x * x);

    if (ix < 0x4036DB8C) {  /* |x| < 1/0.35 */
        R = __erff_Ra(s);
        S = __erff_Sa(s);
    } else {    /* |x| >= 1/0.35 */
        R = __erff_Rb(s);
        S = __erff_Sb(s);
    }

    GET_FLOAT_WORD(ix, x);
    SET_FLOAT_WORD(z, ix & 0xffffc000U);
    r  =  expf(-z * z - 0.5625f) * expf((z - x) * (z + x) + R / S);

    if (hx >= 0) {
        return one - r / x;
    } else {
        return  r / x - one;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double erf(double x)
{
    return (double) erff((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
