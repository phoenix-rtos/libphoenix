/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions splits the input value into integral and fractional
 * part.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float modff(float x, float *iptr);
 *     double modf(double x, double *iptr);
 *     long double modfl(long double x, long double *iptr);
 *
 * Description
 * ===========
 *
 * ``modf`` splits the input value into integral and fractional part, each of
 * which have the same sign.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    modf(x) = x - iptr \wedge \left\{\begin{array}{ll} iptr = \lfloor x \rfloor, & x \geq 0  \\ iptr = \lceil x \rceil, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``modf`` returns the fractional part of :math:`x` in the range
 * :math:`]-1.0,1.0[` and puts the integral part into the output pointer
 * :math:`*iptr`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+----------------------------------+--------------+--------------+----------------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`                       | :math:`-0`   | :math:`+0`   | :math:`>0`                       | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==================================+==============+==============+==================================+==============+==============+
 * | **modf(x)**         | :math:`-0`   | :math:`x - \lceil x \rceil`      | :math:`x`                   | :math:`x - \lfloor x \rfloor`    | :math:`+0`   | :math:`qNaN` |
 * +---------------------+--------------+----------------------------------+--------------+--------------+----------------------------------+--------------+              +
 * | :math:`*iptr`       | :math:`-Inf` | :math:`\lceil x \rceil`          | :math:`x`                   | :math:`\lfloor x \rfloor`        | :math:`+Inf` |              |
 * +---------------------+--------------+----------------------------------+--------------+--------------+----------------------------------+--------------+--------------+
 *
 */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double modf(double x, double *iptr)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double _xi = 0.0;
    int32_t _i0, _i1, _j0;
    uint32_t i;

    assert(iptr != (void*)0);
    if(iptr == (void*)0) {
        iptr = &_xi;
    }

    EXTRACT_WORDS(_i0, _i1, x);
    _j0 = ((_i0 >> 20) & 0x7ff) - 0x3ff;                 /* exponent of x */

    if (_j0 < 20) {                                     /* integer part in high x */
        if (_j0 < 0) {                                  /* |x|<1 */
            INSERT_WORDS(*iptr, _i0 & 0x80000000U, 0U); /* *iptr = +-0 */
            return x;
        } else {
            i = (0x000fffff) >> _j0;

            if (((_i0 & i) | _i1) == 0) {                /* x is integral */
                *iptr = x;
                INSERT_WORDS(x, _i0 & 0x80000000U, 0U); /* return +-0 */
                return x;
            } else {
                INSERT_WORDS(*iptr, _i0 & (~i), 0);
                return x - *iptr;
            }
        }
    } else if (_j0 > 51) {                              /* no fraction part */
        *iptr = x;

        if (isnan(x)) {
            return *iptr = x + x;                      /* x is NaN, return NaN */
        }

        INSERT_WORDS(x, _i0 & 0x80000000U, 0U);         /* return +-0 */
        return x;
    } else {                                           /* fraction part in low x */
        i = ((uint32_t)(0xffffffffU)) >> (_j0 - 20);

        if ((_i1 & i) == 0) {                           /* x is integral */
            *iptr = x;
            INSERT_WORDS(x, _i0 & 0x80000000U, 0U);     /* return +-0 */
            return x;
        } else {
            INSERT_WORDS(*iptr, _i0, _i1 & (~i));
            return x - *iptr;
        }
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double modfl(long double x, long double *iptr)
{
    return (long double) modf((double) x, (double *) iptr);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
