/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the nearest integer value to :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float rintf(float x);
 *     double rint(double x);
 *     long double rintl(long double x);
 *
 * Description
 * ===========
 *
 * ``rint`` computes the nearest integer value to :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    rint(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``rint`` returns the nearest integer value to :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------------------+--------------+--------------+--------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`               | :math:`-0`   | :math:`+0`   | :math:`>0`               | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==========================+==============+==============+==========================+==============+==============+
 * | **rint(x)**         | :math:`-Inf` | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+--------------------------+--------------+--------------+--------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double TWO52[2] = {
     4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
    -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

double rint(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t _i0, _j0, sx;
    uint32_t i, _i1;
    double t;
    volatile double w;
    EXTRACT_WORDS(_i0, _i1, x);
    sx = (_i0 >> 31) & 1;               /* sign */
    _j0 = ((_i0 >> 20) & 0x7ff) - 0x3ff; /* exponent */

    if (_j0 < 20) {                     /* no integral bits in LS part */
        if (_j0 < 0) {                  /* x is fractional or 0 */
            if (((_i0 & 0x7fffffff) | _i1) == 0) {
                return x;              /* x == 0 */
            }

            _i1 |= (_i0 & 0x0fffff);
            _i0 &= 0xfffe0000U;
            _i0 |= ((_i1 | -_i1) >> 12) & 0x80000;
            SET_HIGH_WORD(x, _i0);
            w = TWO52[sx] + x;
            t =  w - TWO52[sx];
            GET_HIGH_WORD(_i0, t);
            SET_HIGH_WORD(t, (_i0 & 0x7fffffff) | (sx << 31));
            return t;
        } else {                       /* x has integer and maybe fraction */
            i = (0x000fffff) >> _j0;

            if (((_i0 & i) | _i1) == 0) {
                return x;              /* x is integral */
            }

            i >>= 1;

            if (((_i0 & i) | _i1) != 0) {
                /* 2nd or any later bit after radix is set */
                if (_j0 == 19) {
                    _i1 = 0x80000000U;
                } else {
                    _i1 = 0;
                }

                _i0 = (_i0 & (~i)) | ((0x40000) >> _j0);
            }
        }
    } else if (_j0 > 51) {
        if (_j0 == 0x400) {
            return x + x;              /* inf or NaN */
        } else {
            return x;                  /* x is integral */
        }
    } else {
        i = ((uint32_t)0xffffffffU) >> (_j0 - 20);

        if ((_i1 & i) == 0) {
            return x;                  /* x is integral */
        }

        i >>= 1;

        if ((_i1 & i) != 0) {
            _i1 = (_i1 & (~i)) | ((0x40000000) >> (_j0 - 20));
        }
    }

    INSERT_WORDS(x, _i0, _i1);
    w = TWO52[sx] + x;
    return w - TWO52[sx];
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double rintl(long double x)
{
    return (long double) rint((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
