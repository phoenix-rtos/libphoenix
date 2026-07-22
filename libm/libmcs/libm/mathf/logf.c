/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float
ln2_hi = 6.9313812256e-01f, /* 0x3f317180 */
ln2_lo = 9.0580006145e-06f, /* 0x3717f7d1 */
two25  = 3.355443200e+07f,  /* 0x4c000000 */
Lg1    = 6.6666668653e-01f, /* 0x3F2AAAAB */
Lg2    = 4.0000000596e-01f, /* 0x3ECCCCCD */
Lg3    = 2.8571429849e-01f, /* 0x3E924925 */
Lg4    = 2.2222198546e-01f, /* 0x3E638E29 */
Lg5    = 1.8183572590e-01f, /* 0x3E3A3325 */
Lg6    = 1.5313838422e-01f, /* 0x3E1CD04F */
Lg7    = 1.4798198640e-01f; /* 0x3E178897 */

static const float zero =  0.0f;

float logf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float hfsq, f, s, z, R, w, t1, t2, dk;
    int32_t k, ix, i, j;

    GET_FLOAT_WORD(ix, x);

    k = 0;

    if (FLT_UWORD_IS_ZERO(ix & 0x7fffffff)) {
        return __raise_div_by_zerof(-1.0f);     /* log(+-0)=-inf */
    }

    if (FLT_UWORD_IS_NAN(ix & 0x7fffffff)) {    /* x = NaN */
        return x + x;
    }

    if (ix < 0) {
        return __raise_invalidf();              /* log(-#) = NaN */
    }

    if (FLT_UWORD_IS_INFINITE(ix)) {            /* x = +Inf */
        return x + x;
    }

    if (FLT_UWORD_IS_SUBNORMAL(ix)) {
        k -= 25;
        x *= two25;                 /* subnormal number, scale up x */
        GET_FLOAT_WORD(ix, x);
    }

    k += (ix >> 23) - 127;
    ix &= 0x007fffff;
    i = (ix + (0x95f64 << 3)) & 0x800000;
    SET_FLOAT_WORD(x, ix | (i ^ 0x3f800000)); /* normalize x or x/2 */
    k += (i >> 23);
    f = x - 1.0f;

    if ((0x007fffff & (15 + ix)) < 16) { /* |f| < 2**-20 */
        if (f == zero) {
            if (k == 0) {
                return zero;
            }  else {
                dk = (float)k;
                return dk * ln2_hi + dk * ln2_lo;
            }
        }

        R = f * f * (0.5f - 0.33333333333333333f * f);

        if (k == 0) {
            return f - R;
        } else {
            dk = (float)k;
            return dk * ln2_hi - ((R - dk * ln2_lo) - f);
        }
    }

    s = f / (2.0f + f);
    dk = (float)k;
    z = s * s;
    i = ix - (0x6147a << 3);
    w = z * z;
    j = (0x6b851 << 3) - ix;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    i |= j;
    R = t2 + t1;

    if (i > 0) {
        hfsq = 0.5f * f * f;

        if (k == 0) {
            return f - (hfsq - s * (hfsq + R));
        } else {
            return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
        }
    } else {
        if (k == 0) {
            return f - s * (f - R);
        } else {
            return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
        }
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double log(double x)
{
    return (double) logf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
