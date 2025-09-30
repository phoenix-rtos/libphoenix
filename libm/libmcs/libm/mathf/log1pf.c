/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/log1pmff.h"

static const float
ln2_hi = 6.9313812256e-01f, /* 0x3f317180 */
ln2_lo = 9.0580006145e-06f; /* 0x3717f7d1 */

static const float zero = 0.0f;

float log1pf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float hfsq, f, c, R, u;
    int32_t k, hx, hu, ax;

    c = NAN; /* initial value of c is never actually used */
    f = NAN; /* initial value of f is never actually used */
    hu = INT_MAX; /* initial value of hu is never actually used */

    GET_FLOAT_WORD(hx, x);
    ax = hx & 0x7fffffff;

    k = 1;

    if (!FLT_UWORD_IS_FINITE(hx)) { /* x = NaN/+Inf */
        return x + x;
    }

    if (hx < 0x3ed413d7) {          /* x < 0.41422  */
        if (ax >= 0x3f800000) {     /* x <= -1.0 */
            if (FLT_UWORD_IS_NAN(ax)) {             /* x = NaN */
                return x + x;
            } else if (x == -1.0f) {
                return __raise_div_by_zerof(-1.0f); /* log1p(-1)=-inf */
            } else {
                return __raise_invalidf();          /* log1p(x<-1)=NaN */
            }
        }

        if (ax < 0x31000000) {      /* |x| < 2**-29 */
            if (ax < 0x24800000) {  /* |x| < 2**-54 */
                return __raise_inexactf(x);
            } else {
                return __raise_inexactf(x - x * x * 0.5f);
            }
        }

        if (hx > 0 || hx <= ((int32_t)0xbe95f61fU)) {
            k = 0;
            f = x;
            hu = 1;
        }    /* -0.2929<x<0.41422 */
    }

    if (k != 0) {
        if (hx < 0x5a000000) {
            u  = 1.0f + x;
            GET_FLOAT_WORD(hu, u);
            k  = (hu >> 23) - 127;
            /* correction term */
            c  = (k > 0) ? 1.0f - (u - x) : x - (u - 1.0f);
            c /= u;
        } else {
            u  = x;
            GET_FLOAT_WORD(hu, u);
            k  = (hu >> 23) - 127;
            c  = 0;
        }

        hu &= 0x007fffff;

        if (hu < 0x3504f7) {
            SET_FLOAT_WORD(u, hu | 0x3f800000); /* normalize u */
        } else {
            k += 1;
            SET_FLOAT_WORD(u, hu | 0x3f000000); /* normalize u/2 */
            hu = (0x00800000 - hu) >> 2;
        }

        f = u - 1.0f;
    }

    hfsq = 0.5f * f * f;

    if (hu == 0) { /* |f| < 2**-20 */
        if (f == zero) {
            c += k * ln2_lo;
            return k * ln2_hi + c;
        }

        R = hfsq * (1.0f - 0.66666666666666666f * f);

        return k * ln2_hi - ((R - (k * ln2_lo + c)) - f);
    }

    if (k == 0) {
        return f - (hfsq - __log1pmff(f));
    } else {
        return k * ln2_hi - ((hfsq - (__log1pmff(f) + (k * ln2_lo + c))) - f);
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double log1p(double x)
{
    return (double) log1pf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
