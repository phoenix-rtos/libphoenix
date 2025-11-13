/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the tangent of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float tanf(float x);
 *     double tan(double x);
 *     long double tanl(long double x);
 *     float __tanf(float x, float y, int k);
 *     double __tan(double x, double y, int k);
 *
 * Description
 * ===========
 *
 * ``tan`` computes the tangent of the input value.
 *
 * ``__tan`` is an internal function that computes the tangent of the input
 * values. The sum of both input parameters :math:`x` and :math:`y` is bounded
 * to [:math:`-\frac{\pi}{4}`, :math:`\frac{\pi}{4}`]. The first parameter
 * :math:`x` is the requested value in raw precision while the second parameter
 * :math:`y` contains a tail for higher precision. If the additional input
 * variable :math:`k` is :math:`-1`, the function shall return the negative
 * inverse tangent of :math:`x`, if :math:`k` is :math:`1` return the tangent.
 * As ``__tan`` is an internal function, it should not be called by a user.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    tan(x) \approx tan(x)
 *
 * Returns
 * =======
 *
 * ``tan`` returns the tangent of the input value.
 *
 * ``__tan`` returns the tangent of the input value.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is infinite.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 * | **x**               | :math:`-Inf`   | :math:`<0`     | :math:`-0`     | :math:`+0`     | :math:`>0`     | :math:`+Inf`   | :math:`NaN`    |
 * +=====================+================+================+================+================+================+================+================+
 * | **tan(x)**          | :math:`qNaN`   | :math:`tan(x)` | :math:`x`                       | :math:`tan(x)` | :math:`qNaN`   | :math:`qNaN`   |
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/trigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
pio4   =  7.85398163397448278999e-01, /* 0x3FE921FB, 0x54442D18 */
pio4lo =  3.06161699786838301793e-17, /* 0x3C81A626, 0x33145C07 */
T[] =  {
     3.33333333333334091986e-01, /* 0x3FD55555, 0x55555563 */
     1.33333333333201242699e-01, /* 0x3FC11111, 0x1110FE7A */
     5.39682539762260521377e-02, /* 0x3FABA1BA, 0x1BB341FE */
     2.18694882948595424599e-02, /* 0x3F9664F4, 0x8406D637 */
     8.86323982359930005737e-03, /* 0x3F8226E3, 0xE96E8493 */
     3.59207910759131235356e-03, /* 0x3F6D6D22, 0xC9560328 */
     1.45620945432529025516e-03, /* 0x3F57DBC8, 0xFEE08315 */
     5.88041240820264096874e-04, /* 0x3F4344D8, 0xF2F26501 */
     2.46463134818469906812e-04, /* 0x3F3026F7, 0x1A8D1068 */
     7.81794442939557092300e-05, /* 0x3F147E88, 0xA03792A6 */
     7.14072491382608190305e-05, /* 0x3F12B80F, 0x32F0A7E9 */
    -1.85586374855275456654e-05, /* 0xBEF375CB, 0xDB605373 */
     2.59073051863633712884e-05, /* 0x3EFB2A70, 0x74BF7AD4 */
};

static inline double __tan(double x, double y, int iy)
{
    double z, r, v, w, s;
    int32_t ix, hx;
    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;  /* high word of |x| */

    if (ix >= 0x3FE59428) {          /* |x|>=0.6744 */
        if (hx < 0) {
            x = -x;
            y = -y;
        }

        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0;
    }

    z    =  x * x;
    w     =  z * z;
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

    if (ix >= 0x3FE59428) {
        v = (double)iy;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }

    if (iy == 1) {
        return w;
    } else {
        /* if allow error up to 2 ulp, simply return -1.0/(x+r) here */
        /*  compute -1.0/(x+r) accurately */
        double a, t;
        z  = w;
        SET_LOW_WORD(z, 0);
        v  = r - (z - x);   /* z+v = r+x */
        t = a  = -1.0 / w;  /* a = -1.0/w */
        SET_LOW_WORD(t, 0);
        s  = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}

double tan(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double y[2], z = 0.0;
    int32_t n, ix;

    /* High word of x. */
    GET_HIGH_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;

    if (ix <= 0x3fe921fb) {
        if(ix < 0x3e400000) {      /* x < 2**-27 */
            if (x == 0.0) {        /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexact(x);
            }
        }

        return __tan(x, z, 1);
    }

    /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7ff00000) {
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalid();
        }
    }

    /* argument reduction needed */
    else {
        n = __rem_pio2(x, y);
        return __tan(y[0], y[1], 1 - ((n & 1) << 1)); /*   1 -- n even, -1 -- n odd */
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double tanl(long double x)
{
    return (long double) tan((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
