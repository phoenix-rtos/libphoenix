/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions splits the input value into mantissa and exponent.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float frexpf(float x, int *eptr);
 *     double frexp(double x, int *eptr);
 *     long double frexpl(long double x, int *eptr);
 *
 * Description
 * ===========
 *
 * ``frexp`` splits the input value into the normalized fraction of :math:`x`
 * and its integral power of :math:`2`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    frexp(x) = \frac{x}{2^{eptr}} \wedge frexp(x) \in [0.5,1[
 *
 * Returns
 * =======
 *
 * ``frexp`` returns the normalized fraction of :math:`x` in the range
 * :math:`[0.5,1[` and puts the integral power of :math:`2` into the output
 * pointer :math:`*eptr`.
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
 * | **frexp(x)**        | :math:`qNaN` | :math:`\frac{x}{2^{eptr}}`       | :math:`x`                   | :math:`\frac{x}{2^{eptr}}`       | :math:`qNaN` | :math:`qNaN` |
 * +---------------------+--------------+----------------------------------+--------------+--------------+----------------------------------+--------------+--------------+
 * | :math:`*eptr`       | :math:`0`    | :math:`log_2 \frac{x}{frexp(x)}` | :math:`0`                   | :math:`log_2 \frac{x}{frexp(x)}` | :math:`0`    | :math:`0`    |
 * +---------------------+--------------+----------------------------------+--------------+--------------+----------------------------------+--------------+--------------+
 *
 */

/*
 * for non-zero x
 *    x = frexp(arg,&exp);
 * return a double fp quantity x such that 0.5 <= |x| <1.0
 * and the corresponding binary exponent "exp". That is
 *    arg = x*2^exp.
 * If arg is inf, 0.0, or NaN, then frexp(arg,&exp) returns arg
 * with *exp=0.
 */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
two54 =  1.80143985094819840000e+16; /* 0x43500000, 0x00000000 */

double frexp(double x, int *eptr)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int _xexp = 0;
    int32_t hx, ix, lx;

    assert(eptr != (void*)0);
    if(eptr == (void*)0) {
        eptr = &_xexp;
    }

    EXTRACT_WORDS(hx, lx, x);
    ix = 0x7fffffff & hx;
    *eptr = 0;

    if (ix >= 0x7ff00000 || ((ix | lx) == 0)) {
        return x + x;           /* 0,inf,nan */
    }

    if (ix < 0x00100000) {      /* subnormal */
        x *= two54;
        GET_HIGH_WORD(hx, x);
        ix = hx & 0x7fffffff;
        *eptr = -54;
    }

    *eptr += (ix >> 20) - 1022;
    hx = (hx & 0x800fffffU) | 0x3fe00000U;
    SET_HIGH_WORD(x, hx);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double frexpl(long double x, int *eptr)
{
    return (long double) frexp((double) x, eptr);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
