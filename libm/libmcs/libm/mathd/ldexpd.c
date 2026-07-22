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
 *     float ldexpf(float x, int exp);
 *     double ldexp(double x, int exp);
 *     long double ldexpl(long double x, int exp);
 *
 * Description
 * ===========
 *
 * ``ldexp`` multiplies the input value :math:`x` by an integral power of :math:`2`.
 *
 * ``ldexp`` and :ref:`scalbn` have the same functionality. In theory their
 * definition could be different, but this only applies to architectures which
 * do not use a binary system, which by now are assumed to be an obscurity.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ldexp(x, exp) \approx x \cdot 2^{exp}
 *
 * Returns
 * =======
 *
 * ``ldexp`` returns the input value :math:`x` multiplied by :math:`2` powered
 * by the input value :math:`exp`.
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
 * | ldexp(x,exp)        | exp                                                                         |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 * | x                   | :math:`<0`              | :math:`0`               | :math:`>0`              |
 * +=====================+=========================+=========================+=========================+
 * | :math:`-Inf`        | :math:`x`               | :math:`x`               | :math:`x`               |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`<0`          | :math:`x \cdot 2^{exp}` |                         | :math:`x \cdot 2^{exp}` |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`-0`          | :math:`x`               |                         | :math:`x`               |
 * +---------------------+                         +                         +                         +
 * | :math:`+0`          |                         |                         |                         |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`>0`          | :math:`x \cdot 2^{exp}` |                         | :math:`x \cdot 2^{exp}` |
 * +---------------------+-------------------------+                         +-------------------------+
 * | :math:`+Inf`        | :math:`x`               |                         | :math:`x`               |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 * | :math:`NaN`         | :math:`qNaN`                                                                |
 * +---------------------+-------------------------+-------------------------+-------------------------+
 *
 */

#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double ldexp(double x, int exp)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    if ((isfinite(x) == 0) || (x == 0.0)) {
        return x + x;
    }

    return scalbn(x, exp);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double ldexpl(long double x, int exp)
{
    return (long double) ldexp((double) x, exp);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
