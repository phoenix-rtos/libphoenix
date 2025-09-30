/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions computes the binary exponent of the input value.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float logbf(float x);
 *     double logb(double x);
 *     long double logbl(long double x);
 *
 * Description
 * ===========
 *
 * ``logb`` computes the binary exponent of the input value.
 *
 * ``logb`` and :ref:`ilogb` have the same functionality, but ``logb`` returns
 * the binary exponent as a floating-point value while :ref:`ilogb` returns a
 * signed integer.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    logb(x) \approx \lfloor {\log_2 |x|} \rfloor
 *
 * Returns
 * =======
 *
 * ``logb`` returns the binary exponent of the input value.
 *
 * Exceptions
 * ==========
 *
 * Raise ``divide by zero`` exception when the input value is zero.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------------------------------+--------------+--------------+--------------------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`                           | :math:`-0`   | :math:`+0`   | :math:`>0`                           | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+======================================+==============+==============+======================================+==============+==============+
 * | **logb(x)**         | :math:`+Inf` | :math:`\lfloor {\log_2 |x|} \rfloor` | :math:`-Inf`                | :math:`\lfloor {\log_2 |x|} \rfloor` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+--------------------------------------+--------------+--------------+--------------------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double logb(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, lx, ix;

    EXTRACT_WORDS(hx, lx, x);
    hx &= 0x7fffffff;        /* high |x| */

    if (hx < 0x00100000) {     /* 0 or subnormal */
        if ((hx | lx) == 0)  {
            return __raise_div_by_zero(-1.0);  /* logb(0) = -inf */
        } else {         /* subnormal x */
            if (hx == 0) {
                for (ix = -1043; lx > 0; lx <<= 1) {
                    ix -= 1;
                }
            } else {
                for (ix = -1022, hx <<= 11; hx > 0; hx <<= 1) {
                    ix -= 1;
                }
            }
        }

        return (double) ix;
    } else if (hx < 0x7ff00000) {
        return (hx >> 20) - 1023;    /* normal # */
    } else {
        return x * x;    /* x = NaN/+-Inf */
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double logbl(long double x)
{
    return (long double) logb((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
