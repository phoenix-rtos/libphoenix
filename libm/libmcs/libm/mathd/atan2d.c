/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the arc tangent of :math:`\frac{y}{x}`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float atan2f(float y, float x);
 *     double atan2(double y, double x);
 *     long double atan2l(long double y, long double x);
 *
 * Description
 * ===========
 *
 * ``atan2`` computes the inverse tangent (*arc tangent*) of the division of
 * the input values.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    atan2(y, x) \approx \left\{\begin{array}{ll}
 *                        tan^{-1}\left(\frac{y}{x}\right), & x > 0  \\
 *                        tan^{-1}\left(\frac{y}{x}\right) + \pi, & x < 0 \wedge y > 0  \\
 *                        tan^{-1}\left(\frac{y}{x}\right) - \pi, & x < 0 \wedge y < 0 \end{array}\right.
 *
 * For the other cases (which do not need formulae) refer to the output map below.
 *
 * Returns
 * =======
 *
 * ``atan2`` returns value in radians, in the range :math:`[-\pi, \pi]`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise overflow, division by zero, and invalid exceptions. Does not raise ``divide-by-zero``
 * exception even if argument :math:`x` is zero.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+--------------+
 * | atan2(y,x)   | y                                                                                                                                                                        |
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+--------------+
 * | x            | :math:`-Inf`             | :math:`<0`                        | :math:`-0`   | :math:`+0`   | :math:`>0`                        | :math:`+Inf`             | :math:`NaN`  |
 * +==============+==========================+===================================+==============+==============+===================================+==========================+==============+
 * | :math:`-Inf` | :math:`-\frac{3}{4} \pi` | :math:`-\pi`                                     | :math:`+\pi`                                     | :math:`+\frac{3}{4} \pi` | :math:`qNaN` |
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+              +
 * | :math:`<0`   | :math:`-\frac{\pi}{2}`   | :math:`tan^{-1}(\frac{y}{x})-\pi` | :math:`-\pi` | :math:`+\pi` | :math:`tan^{-1}(\frac{y}{x})+\pi` | :math:`+\frac{\pi}{2}`   |              |
 * +--------------+                          +-----------------------------------+              +              +-----------------------------------+                          +              +
 * | :math:`-0`   |                          | :math:`-\frac{\pi}{2}`            |              |              | :math:`+\frac{\pi}{2}`            |                          |              |
 * +--------------+                          +                                   +--------------+--------------+                                   +                          +              +
 * | :math:`+0`   |                          |                                   | :math:`-0`   | :math:`+0`   |                                   |                          |              |
 * +--------------+                          +-----------------------------------+              +              +-----------------------------------+                          +              +
 * | :math:`>0`   |                          | :math:`tan^{-1}(\frac{y}{x})`     |              |              | :math:`tan^{-1}(\frac{y}{x})`     |                          |              |
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+              +
 * | :math:`+Inf` | :math:`-\frac{\pi}{4}`   | :math:`-0`                                       | :math:`+0`                                       | :math:`+\frac{\pi}{4}`   |              |
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+--------------+
 * | :math:`NaN`  | :math:`qNaN`                                                                                                                                                             |
 * +--------------+--------------------------+-----------------------------------+--------------+--------------+-----------------------------------+--------------------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
zero    = 0.0,
pi_o_4  = 7.8539816339744827900E-01, /* 0x3FE921FB, 0x54442D18 */
pi_o_2  = 1.5707963267948965580E+00, /* 0x3FF921FB, 0x54442D18 */
pi      = 3.1415926535897931160E+00, /* 0x400921FB, 0x54442D18 */
pi_lo   = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */

double atan2(double y, double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z;
    int32_t k, m, hx, hy, ix, iy;
    uint32_t lx, ly;

    EXTRACT_WORDS(hx, lx, x);
    ix = hx & 0x7fffffff;
    EXTRACT_WORDS(hy, ly, y);
    iy = hy & 0x7fffffff;

    if (((ix | ((lx | -lx) >> 31)) > 0x7ff00000) ||
        ((iy | ((ly | -ly) >> 31)) > 0x7ff00000)) { /* x or y is NaN */
        return x + y;
    }

    if (hx == 0x3ff00000 && lx == 0) {
        return atan(y);    /* x=1.0 */
    }

    m = ((hy >> 31) & 1) | ((hx >> 30) & 2); /* 2*sign(x)+sign(y) */

    /* when y = 0 */
    if ((iy | ly) == 0) {
        switch (m) {
        default:    /* FALLTHRU */
        case 0:     /* FALLTHRU */
        case 1:
            return y;          /* atan(+-0,+anything)=+-0 */

        case 2:
            return __raise_inexact(pi); /* atan(+0,-anything) = pi */

        case 3:
            return -__raise_inexact(pi); /* atan(-0,-anything) =-pi */
        }
    }

    /* when x = 0 */
    if ((ix | lx) == 0) {
        return (hy < 0) ? -__raise_inexact(pi_o_2) : __raise_inexact(pi_o_2);
    }

    /* when x is INF */
    if (ix == 0x7ff00000) {
        if (iy == 0x7ff00000) {
            switch (m) {
            default:    /* FALLTHRU */
            case 0:
                return  __raise_inexact(pi_o_4);        /* atan(+INF,+INF) */

            case 1:
                return -__raise_inexact(pi_o_4);        /* atan(-INF,+INF) */

            case 2:
                return  __raise_inexact(3.0 * pi_o_4);  /* atan(+INF,-INF) */

            case 3:
                return -__raise_inexact(3.0 * pi_o_4);  /* atan(-INF,-INF) */
            }
        } else {
            switch (m) {
            default:    /* FALLTHRU */
            case 0:
                return  zero;                           /* atan(+...,+INF) */

            case 1:
                return -zero;                           /* atan(-...,+INF) */

            case 2:
                return  __raise_inexact(pi);            /* atan(+...,-INF) */

            case 3:
                return -__raise_inexact(pi);            /* atan(-...,-INF) */
            }
        }
    }

    /* when y is INF */
    if (iy == 0x7ff00000) {
        return (hy < 0) ? -__raise_inexact(pi_o_2) : __raise_inexact(pi_o_2);
    }

    /* compute y/x */
    k = (iy - ix) >> 20;

    if (k > 60) {
        z = __raise_inexact(pi_o_2);    /* |y/x| >  2**60 */
        m &= 1;
    } else if (hx < 0 && k < -60) {
        z = 0.0;                        /* 0 > |y|/x > -2**60 */
    } else {
        z = atan(fabs(y / x));          /* safe to do y/x */
    }

    switch (m) {
    case 0:
        return  z;                      /* atan(+,+) */

    case 1:
        return  -z;                     /* atan(-,+) */

    case 2:
        return  pi - (z - pi_lo);       /* atan(+,-) */

    default: /* case 3 */
        return (z - pi_lo) - pi;        /* atan(-,-) */
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double atan2l(long double y, long double x)
{
    return (long double) atan2((double) y, (double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
