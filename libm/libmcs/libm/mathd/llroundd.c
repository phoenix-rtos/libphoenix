/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* lround adapted to be llround for Newlib, 2009 by Craig Howland. */

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
 *     long long int llroundf(float x);
 *     long long int llround(double x);
 *     long long int llroundl(long double x);
 *
 * Description
 * ===========
 *
 * ``llround`` computes the nearest integer value to :math:`x`. Functionally
 * the same procedure as :ref:`lround` but returns ``long long int`` instead of
 * ``long int``.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    llround(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``llround`` returns the nearest integer value to :math:`x`.
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
 * | **llround(x)**      | min :math:`\mathbb{I}_{ll}`                                      | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | max :math:`\mathbb{I}_{ll}`                                      | :math:`llround(±Inf)`    |
 * +---------------------+--------------------------+---------------------------------------+--------------------------+--------------+--------------+--------------------------+---------------------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

long long int llround(double x)
{
    int32_t sign, exponent_less_1023;
    /* Most significant word, least significant word. */
    uint32_t msw, lsw;
    long long int result;

    EXTRACT_WORDS(msw, lsw, x);

    /* Extract sign. */
    sign = ((msw & 0x80000000U) ? -1 : 1);
    /* Extract exponent field. */
    exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;
    msw &= 0x000fffff;
    msw |= 0x00100000;

    /* exponent_less_1023 in [-1023,1024] */
    if (exponent_less_1023 < 20) {
        /* exponent_less_1023 in [-1023,19] */
        if (exponent_less_1023 < 0) {
            if (exponent_less_1023 < -1) {
                return 0;
            } else {
                return sign;
            }
        } else {
            /* exponent_less_1023 in [0,19] */
            /* shift amt in [0,19] */
            msw += 0x80000 >> exponent_less_1023;
            /* shift amt in [20,1] */
            result = msw >> (20 - exponent_less_1023);
        }
    } else if ((uint32_t)exponent_less_1023 < (8 * sizeof(long long int)) - 1) {
        /* 64bit longlong: exponent_less_1023 in [20,62] */
        if (exponent_less_1023 >= 52) {
            /* 64bit longlong: exponent_less_1023 in [52,62] */
            /* 64bit longlong: shift amt in [32,42] */
            result = ((long long int) msw << (exponent_less_1023 - 20))
                     /* 64bit longlong: shift amt in [0,10] */
                     | ((long long int) lsw << (exponent_less_1023 - 52));
        } else {
            /* 64bit longlong: exponent_less_1023 in [20,51] */
            /* 64bit longlong: shift amt in [0,31] */
            uint32_t tmp = lsw + (0x80000000U >> (exponent_less_1023 - 20));

            if (tmp < lsw) {
                ++msw;
            }

            /* 64bit longlong: shift amt in [0,31] */
            result = ((long long int) msw << (exponent_less_1023 - 20))
                     /* ***64bit longlong: shift amt in [32,1] */
                     | SAFE_RIGHT_SHIFT(tmp, (uint32_t)(52 - exponent_less_1023));
        }
    } else {   /* Result is too large to be represented by a long long int. */
        (void) __raise_invalid();
        if (sign == -1) {
            return __MIN_LONG_LONG;
        }
        else {
            return __MAX_LONG_LONG;
        }
    }

    return sign * result;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long long int llroundl(long double x)
{
    return llround((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
