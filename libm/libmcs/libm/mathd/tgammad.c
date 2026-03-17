/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the gamma function of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float tgammaf(float x);
 *     double tgamma(double x);
 *     long double tgammal(long double x);
 *
 * Description
 * ===========
 *
 * ``tgamma`` computes the gamma function of :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    tgamma(x) \approx \Gamma(x) = \int_{0}^{\infty}e^{-t}t^{x-1}dt
 *
 * Returns
 * =======
 *
 * ``tgamma`` returns the gamma function of :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is a negative
 * integer or negative infinity.
 *
 * Raise ``divide by zero`` exception when the input value is zero.
 *
 * Raise ``overflow`` exception when the magnitude of the input value is too
 * large or too small.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+---------------------------------------+-----------------------------+--------------+--------------+-------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0\ \wedge\ \notin \mathbb{Z}` | :math:`\in \mathbb{Z}_{<0}` | :math:`-0`   | :math:`+0`   | :math:`>0`        | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=======================================+=============================+==============+==============+===================+==============+==============+
 * | **tgamma(x)**       | :math:`qNaN` | :math:`\Gamma(x)`                     | :math:`qNaN`                | :math:`-Inf` | :math:`+Inf` | :math:`\Gamma(x)` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+---------------------------------------+-----------------------------+--------------+--------------+-------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/gammad.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double tgamma(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int signgam_local = 0;
    double y = 0.0;

    if (isnan(x) != 0) {                    /* tgamma(NaN) = NaN */
        return x + x;
    } else if (x == 0.0) {                  /* tgamma(+-0) = +-Inf */
        return __raise_div_by_zero(x);
    } else if (floor(x) == x && x < 0.0) {  /* tgamma(negative integer, -Inf) = NaN */
        return __raise_invalid();
    } else {
        /* No action required */
    }

    y = exp(__lgamma(x, &signgam_local));

    if (signgam_local < 0) {
        y = -y;
    }

    return y;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double tgammal(long double x)
{
    return (long double) tgamma((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
