/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* lrint adapted to be llrint for Newlib, 2009 by Craig Howland. */

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
 *     long long int llrintf(float x);
 *     long long int llrint(double x);
 *     long long int llrintl(long double x);
 *
 * Description
 * ===========
 *
 * ``llrint`` computes the nearest integer value to :math:`x`. Functionally the
 * same procedure as :ref:`lrint` but returns ``long long int`` instead of
 * ``long int``.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    llrint(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``llrint`` returns the nearest integer value to :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the correct result is not
 * representable as the output type. This is the case when the input value is
 * infinite or :math:`NaN`, or the magnitude of the result is too large to be
 * represented.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------------------+---------------------------------------+--------------------------+--------------+--------------+--------------------------+---------------------------------------+--------------------------+--------------------------+
 * | **x**               | :math:`-Inf`             | :math:`<` min :math:`\mathbb{I}_{ll}` | :math:`<0`               | :math:`-0`   | :math:`+0`   | :math:`>0`               | :math:`>` max :math:`\mathbb{I}_{ll}` | :math:`+Inf`             | :math:`NaN`              |
 * +=====================+==========================+=======================================+==========================+==============+==============+==========================+=======================================+==========================+==========================+
 * | **llrint(x)**       | min :math:`\mathbb{I}_{ll}`                                      | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | max :math:`\mathbb{I}_{ll}`                                      | :math:`llrint(±Inf)`     |
 * +---------------------+--------------------------+---------------------------------------+--------------------------+--------------+--------------+--------------------------+---------------------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

/* Adding a double, x, to 2^52 will cause the result to be rounded based on
   the fractional part of x, according to the implementation's current rounding
   mode.  2^52 is the smallest double that can be represented using all 52 significant
   digits. */
static const double TWO52[2] = {
     4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
    -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

long long int llrint(double x)
{
    int32_t _i0, _j0, sx;
    uint32_t _i1;
    double t;
    volatile double w;
    long long int result;

    EXTRACT_WORDS(_i0, _i1, x);

    /* Extract sign bit. */
    sx = (_i0 >> 31) & 1;

    /* Extract exponent field. */
    _j0 = ((_i0 & 0x7ff00000) >> 20) - 1023;
    /* _j0 in [-1023,1024] */

    if (_j0 < 20) {
        /* _j0 in [-1023,19] */
        if (_j0 < -1) {
            return 0;
        } else {
            /* _j0 in [0,19] */
            /* shift amt in [0,19] */
            w = TWO52[sx] + x;
            t = w - TWO52[sx];
            GET_HIGH_WORD(_i0, t);

            /* Detect the all-zeros representation of plus and
               minus zero, which fails the calculation below. */
            if ((_i0 & ~((int32_t)1 << 31)) == 0) {
                return 0;
            }

            /* After round:  _j0 in [0,20] */
            _j0 = ((_i0 & 0x7ff00000) >> 20) - 1023;
            _i0 &= 0x000fffff;
            _i0 |= 0x00100000;
            /* shift amt in [20,0] */
            result = _i0 >> (20 - _j0);
        }
    } else if ((uint32_t)_j0 < (8 * sizeof(long long int)) - 1) {
        /* 64bit return: _j0 in [20,62] */
        if (_j0 >= 52) {
            /* 64bit return: _j0 in [52,62] */
            /* 64bit return: left shift amt in [32,42] */
            result = ((long long int)((_i0 & 0x000fffff) | 0x00100000) << (_j0 - 20)) |
                     /* 64bit return: right shift amt in [0,10] */
                     ((long long int) _i1 << (_j0 - 52));
        } else {
            /* 64bit return: _j0 in [20,51] */
            w = TWO52[sx] + x;
            t = w - TWO52[sx];
            EXTRACT_WORDS(_i0, _i1, t);
            _j0 = ((_i0 & 0x7ff00000) >> 20) - 1023;
            _i0 &= 0x000fffff;
            _i0 |= 0x00100000;
            /* After round:
            * 64bit return: _j0 in [20,52] */
            /* 64bit return: left shift amt in [0,32] */
            /* ***64bit return: right shift amt in [32,0] */
            result = ((long long int) _i0 << (_j0 - 20)) | SAFE_RIGHT_SHIFT(_i1, (uint32_t)(52 - _j0));
        }
    } else {
        (void) __raise_invalid();
        if (sx != 0) {
            return __MIN_LONG_LONG;
        }
        else {
            return __MAX_LONG_LONG;
        }
    }

    return (sx != 0) ? -result : result;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long long int llrintl(long double x)
{
    return llrint((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
