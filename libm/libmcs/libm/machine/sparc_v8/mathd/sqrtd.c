/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2017-2022 by GTD GmbH. */

/**
 *
 * This family of functions implements the square root of :math:`x` using the
 * SPARC V8 hardware instruction directly.
 * Use this file (and the float version) to replace the software square root
 * when using a SPARC V8 machine. The replacement shall take place in the
 * Makefile.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float sqrtf(float x);
 *     double sqrt(double x);
 *     long double sqrtl(long double x);
 *
 * Description
 * ===========
 *
 * ``sqrt`` computes the square root of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    sqrt(x) \approx \sqrt{x}
 *
 * Returns
 * =======
 *
 * ``sqrt`` returns the square root of the input value.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is negative.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`       | :math:`-0`   | :math:`+0`   | :math:`>0`       | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==================+==============+==============+==================+==============+==============+
 * | **sqrt(x)**         | :math:`qNaN` | :math:`qNaN`     | :math:`x`                   | :math:`\sqrt{x}` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 *
 */

#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double sqrt(double x)
{
    double root = 0.0;

    __asm__ volatile ("fsqrtd %[x], %[root]"
                      : [root] "=f" (root)
                      : [x] "f" (x));

    return root;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double sqrtl(long double x)
{
    return (long double) sqrt((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
