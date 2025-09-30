/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements rounding towards negative infinity of
 * :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float floorf(float x);
 *     double floor(double x);
 *     long double floorl(long double x);
 *
 * Description
 * ===========
 *
 * ``floor`` computes the input value rounded towards negative infinity.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    floor(x) = \lfloor x \rfloor
 *
 * Returns
 * =======
 *
 * ``floor`` returns the input value rounded towards negative infinity.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+---------------------------+--------------+--------------+---------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`                | :math:`-0`   | :math:`+0`   | :math:`>0`                | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+===========================+==============+==============+===========================+==============+==============+
 * | **floor(x)**        | :math:`-Inf` | :math:`\lfloor x \rfloor` | :math:`x`                   | :math:`\lfloor x \rfloor` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+---------------------------+--------------+--------------+---------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double floor(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t _i0, _i1, _j0;
    uint32_t i, j;
    EXTRACT_WORDS(_i0, _i1, x);
    _j0 = ((_i0 >> 20) & 0x7ff) - 0x3ff;

    if (_j0 < 20) {
        if (_j0 < 0) {  /* raise inexact if x != 0 */
            if (((_i0 & 0x7fffffff) | _i1) == 0) {
                return x;
            }

            (void) __raise_inexact(x);

            if (_i0 >= 0) {
                _i0 = _i1 = 0;
            } else {
                _i0 = (int32_t)0xbff00000U;
                _i1 = 0;
            }
        } else {
            i = (0x000fffff) >> _j0;

            if (((_i0 & i) | _i1) == 0) {
                return x;    /* x is integral */
            }

            (void) __raise_inexact(x);

            if (_i0 < 0) {
                _i0 += (0x00100000) >> _j0;
            }

            _i0 &= (~i);
            _i1 = 0;
        }
    } else if (_j0 > 51) {
        if (_j0 == 0x400) {
            return x + x;    /* inf or NaN */
        } else {
            return x;    /* x is integral */
        }
    } else {
        i = ((uint32_t)0xffffffffU) >> (_j0 - 20);

        if ((_i1 & i) == 0) {
            return x;    /* x is integral */
        }

        (void) __raise_inexact(x);

        if (_i0 < 0) {
            if (_j0 == 20) {
                _i0 += 1;
            } else {
                j = _i1 + (1 << (52 - _j0));

                if (j < (uint32_t)_i1) {
                    _i0 += 1 ;    /* got a carry */
                }

                _i1 = j;
            }
        }

        _i1 &= (~i);
    }

    INSERT_WORDS(x, _i0, _i1);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double floorl(long double x)
{
    return (long double) floor((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
