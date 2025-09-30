/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This function implements the Bessel function of the first kind of
 * order :math:`n`.
 *
 * .. warning::
 *    The implementation contains a potentially unbounded while loop.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     double jn(int n, double x);
 *
 * Description
 * ===========
 *
 * ``jn`` computes the Bessel value of :math:`x` of the first kind of order
 * :math:`n`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    jn(n, x) = J_{n}(x)
 *
 * Notice that the mathematical function represented by the procedure ``jn`` is
 * not :math:`j_n` (which is the spherical version of the Bessel function) but
 * :math:`J_n`. See `WolframAlpha Jn(x)
 * <https://www.wolframalpha.com/input/?i=Jn%28x%29>`_ for what it looks like
 * (it shows the different :math:`n` quite nicely in a 3D plot) and `Wikipedia
 * <https://en.wikipedia.org/wiki/Bessel_function>`_ for more information.
 *
 * Returns
 * =======
 *
 * ``jn`` returns the Bessel value of :math:`x` of the first kind of order
 * :math:`n`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise overflow, division by zero, and invalid exceptions.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+
 * | jn(n,x)                  | n                                                   |
 * +--------------------------+--------------------------+--------------------------+
 * | x                        | :math:`<0`               | :math:`>0`               |
 * +==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`+0`                                          |
 * +--------------------------+--------------------------+--------------------------+
 * | :math:`<0`               | :math:`J_{-n}(-x)`       | :math:`J_{n}(x)`         |
 * +--------------------------+                          +                          +
 * | :math:`-0`               |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`+0`               |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`>0`               |                          |                          |
 * +--------------------------+--------------------------+--------------------------+
 * | :math:`+Inf`             | :math:`+0`                                          |
 * +--------------------------+--------------------------+--------------------------+
 * | :math:`NaN`              | :math:`qNaN`                                        |
 * +--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "internal/besseld.h"
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double jn(int n, double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t i, hx, ix, lx, sgn;
    double a, b, temp, di;
    double z, w;

    /* J(-n,x) = (-1)^n * J(n, x), J(n, -x) = (-1)^n * J(n, x)
     * Thus, J(-n,x) = J(n,-x)
     */
    EXTRACT_WORDS(hx, lx, x);
    ix = 0x7fffffff & hx;

    /* if J(n,NaN) is NaN */
    if ((ix | ((uint32_t)(lx | -lx)) >> 31) > 0x7ff00000) {
        return x + x;
    }

    if (n < 0) {
        n = -n;
        x = -x;
        hx ^= 0x80000000U;
    }

    if (n == 0) {
        return (j0(x));
    }

    if (n == 1) {
        return (j1(x));
    }

    sgn = (n & 1) & (hx >> 31); /* even n -- 0, odd n -- sign(x) */
    x = fabs(x);

    if ((ix | lx) == 0 || ix >= 0x7ff00000) { /* if x is 0 or inf */
        b = zero;
    } else if ((double)n <= x) {
        /* Safe to use J(n+1,x)=2n/x *J(n,x)-J(n-1,x) */
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
                temp =  cos(x) + sin(x);
                break;

            case 1:
                temp = -cos(x) + sin(x);
                break;

            case 2:
                temp = -cos(x) - sin(x);
                break;

            case 3:
                temp =  cos(x) - sin(x);
                break;
            }

            b = invsqrtpi * temp / sqrt(x);
        } else {
            a = j0(x);
            b = j1(x);

            for (i = 1; i < n; i++) {
                temp = b;
                b = b * ((double)(i + i) / x) - a; /* avoid underflow */
                a = temp;
            }
        }
    } else {
        if (ix < 0x3e100000) { /* x < 2**-29 */
            /* x is tiny, return the first Taylor expansion of J(n,x)
             * J(n,x) = 1/n!*(x/2)^n  - ...
             */
            if (n > 33) { /* underflow */
                b = zero;
            } else {
                temp = x * 0.5;
                b = temp;

                for (a = one, i = 2; i <= n; i++) {
                    a *= (double)i;        /* a = n! */
                    b *= temp;        /* b = (x/2)^n */
                }

                b = b / a;
            }
        } else {
            /* use backward recurrence */
            /*             x      x^2      x^2
             *  J(n,x)/J(n-1,x) =  ----   ------   ------   .....
             *            2n  - 2(n+1) - 2(n+2)
             *
             *             1      1        1
             *  (for large x)   =  ----  ------   ------   .....
             *            2n   2(n+1)   2(n+2)
             *            -- - ------ - ------ -
             *             x     x         x
             *
             * Let w = 2n/x and h=2/x, then the above quotient
             * is equal to the continued fraction:
             *            1
             *    = -----------------------
             *               1
             *       w - -----------------
             *              1
             *             w+h - ---------
             *               w+2h - ...
             *
             * To determine how many terms needed, let
             * Q(0) = w, Q(1) = w(w+h) - 1,
             * Q(k) = (w+k*h)*Q(k-1) - Q(k-2),
             * When Q(k) > 1e4    good for single
             * When Q(k) > 1e9    good for double
             * When Q(k) > 1e17    good for quadruple
             */
            /* determine k */
            double t, v;
            double q0, q1, h, tmp;
            int32_t k, m;
            w  = (n + n) / (double)x;
            h = 2.0 / (double)x;
            q0 = w;
            z = w + h;
            q1 = w * z - 1.0;
            k = 1;

            while (q1 < 1.0e9) {
                k += 1;
                z += h;
                tmp = z * q1 - q0;
                q0 = q1;
                q1 = tmp;
            }

            m = n + n;

            for (t = zero, i = 2 * (n + k); i >= m; i -= 2) {
                t = one / (i / x - t);
            }

            a = t;
            b = one;
            /*  estimate log((2/x)^n*n!) = n*log(2/x)+n*ln(n)
             *  Hence, if n*(log(2n/x)) > ...
             *  single 8.8722839355e+01
             *  double 7.09782712893383973096e+02
             *  long double 1.1356523406294143949491931077970765006170e+04
             *  then recurrent value may overflow and the result is
             *  likely underflow to zero
             */
            tmp = n;
            v = two / x;
            tmp = tmp * log(fabs(v * tmp));

            if (tmp < 7.09782712893383973096e+02) {
                for (i = n - 1, di = (double)(i + i); i > 0; i--) {
                    temp = b;
                    b *= di;
                    b  = b / x - a;
                    a = temp;
                    di -= two;
                }
            } else {
                for (i = n - 1, di = (double)(i + i); i > 0; i--) {
                    temp = b;
                    b *= di;
                    b  = b / x - a;
                    a = temp;
                    di -= two;

                    /* scale b to avoid spurious overflow */
                    if (b > 1e100) {
                        a /= b;
                        t /= b;
                        b  = one;
                    }
                }
            }

            b = (t * j0(x) / b);
        }
    }

    if (sgn != 0) {
        return -b;
    } else {
        return b;
    }
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
