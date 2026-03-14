/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the natural logarithm of the gamma
 * function of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float lgammaf(float x);
 *     double lgamma(double x);
 *     long double lgammal(long double x);
 *
 * Description
 * ===========
 *
 * ``lgamma`` computes the natural logarithm of the gamma function of :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    lgamma(x) \approx \ln{|\Gamma(x)|} = \ln{\left|\int_{0}^{\infty}e^{-t}t^{x-1}dt\right|}
 *
 * Returns
 * =======
 *
 * ``lgamma`` returns the natural logarithm of the gamma function of :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``divide by zero`` exception when the input value is a negative
 * integer or zero.
 *
 * Raise ``overflow`` exception when the magnitude of the input value is too
 * large.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+---------------------------------------+-----------------------------+--------------+--------------+--------------------------+--------------+--------------------------+--------------+--------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0\ \wedge\ \notin \mathbb{Z}` | :math:`\in \mathbb{Z}_{<0}` | :math:`-0`   | :math:`+0`   | :math:`]0,1[`            | :math:`+1`   | :math:`]1,2[`            | :math:`+2`   | :math:`>2`               | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=======================================+=============================+==============+==============+==========================+==============+==========================+==============+==========================+==============+==============+
 * | **lgamma(x)**       | :math:`+Inf` | :math:`\ln{|\Gamma(x)|}`              | :math:`+Inf`                | :math:`+Inf`                | :math:`\ln{|\Gamma(x)|}` | :math:`+0`   | :math:`\ln{|\Gamma(x)|}` | :math:`+0`   | :math:`\ln{|\Gamma(x)|}` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+---------------------------------------+-----------------------------+--------------+--------------+--------------------------+--------------+--------------------------+--------------+--------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "internal/gammad.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double lgamma(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return __lgamma(x, &__signgam);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double lgammal(long double x)
{
    return (long double) lgamma((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
