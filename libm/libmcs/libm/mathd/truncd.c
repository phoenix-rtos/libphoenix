/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the nearest integer value towards zero
 * from :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float truncf(float x);
 *     double trunc(double x);
 *     long double truncl(long double x);
 *
 * Description
 * ===========
 *
 * ``trunc`` computes the nearest integer value towards zero from :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    trunc(x) = \left\{\begin{array}{ll} \lfloor x \rfloor, & x \in \mathbb{F}^{+} \\ \lceil x \rceil, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``trunc`` returns the nearest integer value towards zero from :math:`x`.
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
 * | **trunc(x)**        | :math:`-Inf` | :math:`\lceil x \rceil`   | :math:`x`                   | :math:`\lfloor x \rfloor` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+---------------------------+--------------+--------------+---------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double trunc(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t sb;
    /* Most significant word, least significant word. */
    uint32_t msw;
    uint32_t lsw;
    int32_t exponent_less_1023;

    EXTRACT_WORDS(msw, lsw, x);

    /* Extract sign bit. */
    sb = msw & 0x80000000U;

    /* Extract exponent field. */
    exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;

    if (exponent_less_1023 < 20) {
        /* All significant digits are in msw. */
        if (exponent_less_1023 < 0) {
            /* -1 < x < 1, so result is +0 or -0. */
            INSERT_WORDS(x, sb, 0);
        } else {
            /* All relevant fraction bits are in msw, so lsw of the result is 0. */
            INSERT_WORDS(x, sb | (msw & ~(0x000fffff >> exponent_less_1023)), 0);
        }
    } else if (exponent_less_1023 > 51) {
        if (exponent_less_1023 == 1024) {
            /* x is infinite, or not a number, so trigger an exception. */
            return x + x;
        }

        /* All bits in the fraction fields of the msw and lsw are needed in the result. */
    } else {
        /* All fraction bits in msw are relevant.  Truncate irrelevant bits from lsw. */
        INSERT_WORDS(x, msw, lsw & ~(0xffffffffU >> (exponent_less_1023 - 20)));
    }

    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double truncl(long double x)
{
    return (long double) trunc((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
