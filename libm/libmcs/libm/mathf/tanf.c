/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/trigf.h"

static const float
pio4   =  7.8539812565e-01f, /* 0x3f490fda */
pio4lo =  3.7748947079e-08f, /* 0x33222168 */
T[]    =  {
          3.3333334327e-01f, /* 0x3eaaaaab */
          1.3333334029e-01f, /* 0x3e088889 */
          5.3968254477e-02f, /* 0x3d5d0dd1 */
          2.1869488060e-02f, /* 0x3cb327a4 */
          8.8632395491e-03f, /* 0x3c11371f */
          3.5920790397e-03f, /* 0x3b6b6916 */
          1.4562094584e-03f, /* 0x3abede48 */
          5.8804126456e-04f, /* 0x3a1a26c8 */
          2.4646313977e-04f, /* 0x398137b9 */
          7.8179444245e-05f, /* 0x38a3f445 */
          7.1407252108e-05f, /* 0x3895c07a */
         -1.8558637748e-05f, /* 0xb79bae5f */
          2.5907305826e-05f, /* 0x37d95384 */
};

static inline float __tanf(float x, float y, int iy)
{
    float z, r, v, w, s;
    int32_t ix, hx;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;  /* high word of |x| */

    if (ix >= 0x3f2ca140) {          /* |x|>=0.6744 */
        if (hx < 0) {
            x = -x;
            y = -y;
        }

        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0f;
    }

    z =  x * x;
    w =  z * z;
    /* Break x^5*(T[1]+x^2*T[2]+...) into
     *      x^5(T[1]+x^4*T[3]+...+x^20*T[11]) +
     *      x^5(x^2*(T[2]+x^4*T[4]+...+x^22*[T12]))
     */
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T[0] * s;
    w = x + r;

    if (ix >= 0x3f2ca140) {
        v = (float)iy;
        return (float)(1 - ((hx >> 30) & 2)) * (v - 2.0f * (x - (w * w / (w + v) - r)));
    }

    if (iy == 1) {
        return w;
    } else {
        /* if allow error up to 2 ulp, simply return -1.0/(x+r) here */
        /*  compute -1.0/(x+r) accurately */
        float a, t;
        int32_t i;
        z  = w;
        GET_FLOAT_WORD(i, z);
        SET_FLOAT_WORD(z, i & 0xfffff000U);
        v  = r - (z - x);   /* z+v = r+x */
        t = a  = -1.0f / w;  /* a = -1.0/w */
        GET_FLOAT_WORD(i, t);
        SET_FLOAT_WORD(t, i & 0xfffff000U);
        s  = 1.0f + t * z;
        return t + a * (s + t * v);
    }
}

float tanf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float y[2], z = 0.0;
    int32_t n, ix;

    GET_FLOAT_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;

    if (ix <= 0x3f490fda) {
        if(ix < 0x39800000) {        /* |x| < 2**-12 */
            if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexactf(x);
            }
        }
        return __tanf(x, z, 1);
    }

    /* tan(Inf or NaN) is NaN */
    else if (!FLT_UWORD_IS_FINITE(ix)) {
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalidf();
        }
    }

    /* argument reduction needed */
    else {
        n = __rem_pio2f(x, y);
        return __tanf(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even, -1 -- n odd */
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double tan(double x)
{
    return (double) tanf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
