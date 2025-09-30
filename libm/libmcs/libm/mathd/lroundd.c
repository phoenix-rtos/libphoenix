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
 *     long int lroundf(float x);
 *     long int lround(double x);
 *     long int lroundl(long double x);
 *
 * Description
 * ===========
 *
 * ``lround`` computes the nearest integer value to :math:`x`. Functionally the
 * same procedure as :ref:`round` but returns ``long int`` instead of a
 * floating point value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    lround(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``lround`` returns the nearest integer value to :math:`x`.
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
 * +---------------------+--------------------------+------------------------------------+--------------------------+--------------+--------------+--------------------------+------------------------------------+--------------------------+--------------------------+
 * | **x**               | :math:`-Inf`             | :math:`<` min :math:`\mathbb{I}_l` | :math:`<0`               | :math:`-0`   | :math:`+0`   | :math:`>0`               | :math:`>` max :math:`\mathbb{I}_l` | :math:`+Inf`             | :math:`NaN`              |
 * +=====================+==========================+====================================+==========================+==============+==============+==========================+====================================+==========================+==========================+
 * | **lround(x)**       | min :math:`\mathbb{I}_l`                                      | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | max :math:`\mathbb{I}_l`                                      | :math:`lround(±Inf)`     |
 * +---------------------+--------------------------+------------------------------------+--------------------------+--------------+--------------+--------------------------+------------------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

long int lround(double x)
{
    int32_t sign, exponent_less_1023;
    /* Most significant word, least significant word. */
    uint32_t msw, lsw;
    long int result;

    EXTRACT_WORDS(msw, lsw, x);

    /* Extract sign. */
    sign = ((msw & 0x80000000U) != 0) ? -1 : 1;
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
    } else if ((uint32_t)exponent_less_1023 < (8 * sizeof(long int)) - 1) {
        /* 32bit long: exponent_less_1023 in [20,30] */
        /* 64bit long: exponent_less_1023 in [20,62] */
        if (exponent_less_1023 >= 52) {
            /* 64bit long: exponent_less_1023 in [52,62] */
            /* 64bit long: shift amt in [32,42] */
            result = ((long int) msw << (exponent_less_1023 - 20))
                     /* 64bit long: shift amt in [0,10] */
                     | ((long int) lsw << (exponent_less_1023 - 52));
        } else {
            /* 32bit long: exponent_less_1023 in [20,30] */
            /* 64bit long: exponent_less_1023 in [20,51] */
            /* 32bit long: shift amt in [0,10] */
            /* 64bit long: shift amt in [0,31] */
            uint32_t tmp = lsw + (0x80000000U >> (exponent_less_1023 - 20));

            if (tmp < lsw) {
                ++msw;
            }

            /* 32bit long: shift amt in [0,10] */
            /* 64bit long: shift amt in [0,31] */
            result = ((long int) msw << (exponent_less_1023 - 20))
                     /* ***32bit long: shift amt in [32,22] */
                     /* ***64bit long: shift amt in [32,1] */
                     | SAFE_RIGHT_SHIFT(tmp, (uint32_t)(52 - exponent_less_1023));
        }
    } else {   /* Result is too large to be represented by a long int. */
        (void) __raise_invalid();
        if (sign == -1) {
            return __MIN_LONG;
        }
        else {
            return __MAX_LONG;
        }
    }

    return sign * result;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long int lroundl(long double x)
{
    return lround((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
