/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions multiplies the input value :math:`x` by an integral
 * power of :math:`2`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float scalblnf(float x, int n);
 *     double scalbln(double x, int n);
 *     long double scalblnl(long double x, int n);
 *
 * Description
 * ===========
 *
 * ``scalbln`` multiplies the input value :math:`x` by an integral power of
 * :math:`2`.
 *
 * ``scalbln`` and :ref:`scalbn` have the same functionality. The difference is
 * that :ref:`scalbn` uses an ``int`` for :math:`n`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    scalbln(x, n) \approx x \cdot 2^{n}
 *
 * Returns
 * =======
 *
 * ``scalbln`` returns the input value :math:`x` multiplied by :math:`2`
 * powered by the input value :math:`n`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception if the result overflows.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+-------------------------+-------------------------+-------------------------+
 * | scalbln(x,n)        | n                                                                           |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 * | x                   | :math:`<0`              | :math:`0`               | :math:`>0`              |
 * +=====================+=========================+=========================+=========================+
 * | :math:`-Inf`        | :math:`x`               | :math:`x`               | :math:`x`               |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`<0`          | :math:`x \cdot 2^{n}`   |                         | :math:`x \cdot 2^{n}`   |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`-0`          | :math:`x`               |                         | :math:`x`               |
 * +---------------------+                         +                         +                         +
 * | :math:`+0`          |                         |                         |                         |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`>0`          | :math:`x \cdot 2^{n}`   |                         | :math:`x \cdot 2^{n}`   |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`+Inf`        | :math:`x`               |                         | :math:`x`               |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 * | :math:`NaN`         | :math:`qNaN`                                                                |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17; /* 0x3C900000, 0x00000000 */

double scalbln(double x, long int n)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t k, hx, lx;
    EXTRACT_WORDS(hx, lx, x);
    k = (hx & 0x7ff00000) >> 20;    /* extract exponent */

    if (k == 0) {                   /* 0 or subnormal x */
        if ((lx | (hx & 0x7fffffff)) == 0) {
            return x;               /* +-0 */
        }

        x *= two54;
        GET_HIGH_WORD(hx, x);
        k = ((hx & 0x7ff00000) >> 20) - 54;
    }

    if (k == 0x7ff) {
        return x + x;               /* NaN or Inf */
    }

    if (n > 50000) {
        return __raise_overflow(x);     /*overflow*/
    }

    k = k + n;

    if (k >  0x7fe) {
        return __raise_overflow(x);     /*overflow*/
    }

    if (n < -50000) {
        return __raise_underflow(x);    /*underflow*/
    }

    if (k > 0) {                    /* normal result */
        SET_HIGH_WORD(x, (hx & 0x800fffffU) | (k << 20U));
        return x;
    }

    if (k <= -54) {
        return __raise_underflow(x);    /*underflow*/
    }

    k += 54;                        /* subnormal result */
    SET_HIGH_WORD(x, (hx & 0x800fffffU) | (k << 20U));
    return x * twom54;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double scalblnl(long double x, long int n)
{
    return (long double) scalbln((double) x, n);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
