/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

float hypotf(float x, float y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float a = x;
    float b = y;
    float t1, t2;
    float _y1, _y2;
    float w;
    int32_t j, k, ha, hb;

    GET_FLOAT_WORD(ha, x);
    ha &= 0x7fffffff;
    GET_FLOAT_WORD(hb, y);
    hb &= 0x7fffffff;

    if (hb > ha) {
        j = ha;
        ha = hb;
        hb = j;
    }

    SET_FLOAT_WORD(a, ha);   /* a <- |a| */
    SET_FLOAT_WORD(b, hb);   /* b <- |b| */

    if ((ha - hb) > 0xf000000) {
        return a + b;   /* x/y > 2**30 */
    }

    k = 0;

    if (ha > 0x58800000) {   /* a>2**50 */
        if (!FLT_UWORD_IS_FINITE(ha)) {   /* Inf or NaN */
            w = a + b;          /* for sNaN */

            if (FLT_UWORD_IS_INFINITE(ha)) {
                w = a;
            }

            if (FLT_UWORD_IS_INFINITE(hb)) {
                w = b;
            }

            return w;
        }

        /* scale a and b by 2**-68 */
        ha -= 0x22000000;
        hb -= 0x22000000;
        k += 68;
        SET_FLOAT_WORD(a, ha);
        SET_FLOAT_WORD(b, hb);
    }

    if (hb < 0x26800000) {   /* b < 2**-50 */
        if (FLT_UWORD_IS_ZERO(hb)) {
            return a;
        } else if (FLT_UWORD_IS_SUBNORMAL(hb)) {
            SET_FLOAT_WORD(t1, 0x7e800000);   /* t1=2^126 */
            b *= t1;
            a *= t1;
            k -= 126;
        } else {        /* scale a and b by 2^80 */
            ha += 0x28000000;     /* a *= 2^80 */
            hb += 0x28000000;    /* b *= 2^80 */
            k -= 80;
            SET_FLOAT_WORD(a, ha);
            SET_FLOAT_WORD(b, hb);
        }
    }

    /* medium size a and b */
    w = a - b;

    if (w > b) {
        SET_FLOAT_WORD(t1, ha & 0xfffff000U);
        t2 = a - t1;
        w  = sqrtf(t1 * t1 - (b * (-b) - t2 * (a + t1)));
    } else {
        a  = a + a;
        SET_FLOAT_WORD(_y1, hb & 0xfffff000U);
        _y2 = b - _y1;
        SET_FLOAT_WORD(t1, (ha + 0x00800000) & 0xfffff000U);
        t2 = a - t1;
        w  = sqrtf(t1 * _y1 - (w * (-w) - (t1 * _y2 + t2 * b)));
    }

    if (k != 0) {
        SET_FLOAT_WORD(t1, (0x7F + k) << 23);
        return t1 * w;
    } else {
        return w;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double hypot(double x, double y)
{
    return (double) hypotf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
