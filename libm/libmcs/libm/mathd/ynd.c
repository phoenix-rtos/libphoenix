/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the Bessel function of the second kind
 * of order :math:`n`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     double yn(int n, double x);
 *
 * Description
 * ===========
 *
 * ``yn`` computes the Bessel value of :math:`x` of the second kind of order
 * :math:`n`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    yn(n, x) = Y_{n}(x)
 *
 * Notice that the mathematical function represented by the procedure ``yn`` is
 * not :math:`y_n` (which is the spherical version of the Bessel function) but
 * :math:`Y_n`. See `Wikipedia
 * <https://en.wikipedia.org/wiki/Bessel_function>`_ for more information.
 *
 * Returns
 * =======
 *
 * ``yn`` returns the Bessel value of :math:`x` of the second kind of order
 * :math:`n`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is negative.
 *
 * Raise ``divide by zero`` exception when :math:`x` is zero.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+-------------------------------+-------------------------------+
 * | yn(n,x)                  | n                                                             |
 * +--------------------------+-------------------------------+-------------------------------+
 * | x                        | :math:`<0`                    | :math:`>0`                    |
 * +==========================+===============================+===============================+
 * | :math:`-Inf`             | :math:`qNaN`                                                  |
 * +--------------------------+                                                               +
 * | :math:`<0`               |                                                               |
 * +--------------------------+-------------------------------+-------------------------------+
 * | :math:`-0`               | :math:`-Inf`                                                  |
 * +--------------------------+                                                               +
 * | :math:`+0`               |                                                               |
 * +--------------------------+-------------------------------+-------------------------------+
 * | :math:`>0`               | :math:`(-1)^n \cdot Y_{n}(x)` | :math:`Y_{n}(x)`              |
 * +--------------------------+-------------------------------+-------------------------------+
 * | :math:`+Inf`             | :math:`+0`                                                    |
 * +--------------------------+-------------------------------+-------------------------------+
 * | :math:`NaN`              | :math:`qNaN`                                                  |
 * +--------------------------+-------------------------------+-------------------------------+
 *
 */

#include <math.h>
#include "internal/besseld.h"
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double yn(int n, double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t i, hx, ix, lx;
    int32_t sign;
    double a, b, temp;

    EXTRACT_WORDS(hx, lx, x);
    ix = 0x7fffffff & hx;

    /* if Y(n,NaN) is NaN */
    if (isnan(x)) {         /* yn(n,NaN) = NaN */
        return x + x;
    }

    if ((ix | lx) == 0) {   /* yn(n,+-0) = +Inf */
        return __raise_div_by_zero(-1.0);
    }

    if (hx < 0) {           /* yn(n,<0) = NaN, y1(n,-Inf) = NaN */
        return __raise_invalid();
    }

    sign = 1;

    if (n < 0) {
        n = -n;
        sign = 1 - ((n & 1) << 1);
    }

    if (n == 0) {
        return (y0(x));
    }

    if (n == 1) {
        return (sign * y1(x));
    }

    if (ix == 0x7ff00000) { /* yn(n,+Inf) = +0.0 */
        return zero;
    }

    if (ix >= 0x52D00000) { /* x > 2**302 */
        /* (x >> n**2)
         *        Jn(x) = cos(x-(2n+1)*pi/4)*sqrt(2/x*pi)
         *        Yn(x) = sin(x-(2n+1)*pi/4)*sqrt(2/x*pi)
         *        Let s=sin(x), c=cos(x),
         *        xn=x-(2n+1)*pi/4, sqt2 = sqrt(2),then
         *
         *           n    sin(xn)*sqt2    cos(xn)*sqt2
         *        ----------------------------------
         *           0     s-c         c+s
         *           1    -s-c         -c+s
         *           2    -s+c        -c-s
         *           3     s+c         c-s
         */
        switch (n & 3) {
        default:    /* FALLTHRU */
        case 0:
            temp =  sin(x) - cos(x);
            break;

        case 1:
            temp = -sin(x) - cos(x);
            break;

        case 2:
            temp = -sin(x) + cos(x);
            break;

        case 3:
            temp =  sin(x) + cos(x);
            break;
        }

        b = invsqrtpi * temp / sqrt(x);
    } else {
        uint32_t high;
        a = y0(x);
        b = y1(x);
        /* quit if b is -inf */
        GET_HIGH_WORD(high, b);

        for (i = 1; i < n && high != 0xfff00000U; i++) {
            temp = b;
            b = ((double)(i + i) / x) * b - a;
            GET_HIGH_WORD(high, b);
            a = temp;
        }
    }

    if (sign > 0) {
        return b;
    } else {
        return -b;
    }
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
