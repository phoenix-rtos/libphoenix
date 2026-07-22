/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements :math:`2` powered by :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float exp2f(float x);
 *     double exp2(double x);
 *     long double exp2l(long double x);
 *
 * Description
 * ===========
 *
 * ``exp2`` computes :math:`2` powered by the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    exp2(x) \approx 2^x
 *
 * Returns
 * =======
 *
 * ``exp2`` returns :math:`2` powered by :math:`x`, in the range
 * :math:`\mathbb{F}^{+}_0`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of the input value is too
 * large.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`   | :math:`-0`   | :math:`+0`   | :math:`>0`   | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==============+==============+==============+==============+==============+==============+
 * | **exp2(x)**         | :math:`+0`   | :math:`2^x`  | :math:`+1`                  | :math:`2^x`  | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double exp2(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return pow(2.0, x);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double exp2l(long double x)
{
    return (long double) exp2((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
