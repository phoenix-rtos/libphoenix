/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float
one     =  1.0000000000e+00f, /* 0x3F800000 */
pi      =  3.1415925026e+00f, /* 0x40490fda */
pio2_hi =  1.5707962513e+00f, /* 0x3fc90fda */
pio2_lo =  7.5497894159e-08f, /* 0x33a22168 */
pS0     =  1.6666667163e-01f, /* 0x3e2aaaab */
pS1     = -3.2556581497e-01f, /* 0xbea6b090 */
pS2     =  2.0121252537e-01f, /* 0x3e4e0aa8 */
pS3     = -4.0055535734e-02f, /* 0xbd241146 */
pS4     =  7.9153501429e-04f, /* 0x3a4f7f04 */
pS5     =  3.4793309169e-05f, /* 0x3811ef08 */
qS1     = -2.4033949375e+00f, /* 0xc019d139 */
qS2     =  2.0209457874e+00f, /* 0x4001572d */
qS3     = -6.8828397989e-01f, /* 0xbf303361 */
qS4     =  7.7038154006e-02f; /* 0x3d9dc62e */

float acosf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float z, p, q, r, w, s, c, df;
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix == 0x3f800000) {     /* |x|==1 */
        if (hx > 0) {
            return 0.0f;    /* acos(1) = 0  */
        } else {
            return pi + 2.0f * pio2_lo;    /* acos(-1)= pi */
        }
    } else if (ix > 0x3f800000) { /* |x| >= 1 */
        if (isnan(x)) {
            return x + x;
        }

        return __raise_invalidf();  /* acos(|x|>1) is NaN */
    } else {
        /* No action required */
    }

    if (ix < 0x3f000000) { /* |x| < 0.5 */
        if (ix <= 0x23000000) {
            return pio2_hi + pio2_lo;    /*if|x|<2**-57*/
        }

        z = x * x;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        return pio2_hi - (x - (pio2_lo - x * r));
    } else if (hx < 0) {      /* x < -0.5 */
        z = (one + x) * 0.5f;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        s = sqrtf(z);
        r = p / q;
        w = r * s - pio2_lo;
        return pi - 2.0f * (s + w);
    } else {            /* x > 0.5 */
        int32_t idf;
        z = (one - x) * 0.5f;
        s = sqrtf(z);
        df = s;
        GET_FLOAT_WORD(idf, df);
        SET_FLOAT_WORD(df, idf & 0xfffff000U);
        c  = (z - df * df) / (s + df);
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        w = r * s + c;
        return 2.0f * (df + w);
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double acos(double x)
{
    return (double) acosf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
