/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the error function of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float erff(float x);
 *     double erf(double x);
 *     long double erfl(long double x);
 *
 * Description
 * ===========
 *
 * ``erf`` computes the error function of :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    erf(x) \approx \frac{2}{\sqrt{\pi}}\int_{0}^{x}e^{-t^2}dt
 *
 * Returns
 * =======
 *
 * ``erf`` returns the error function of :math:`x`, in the range
 * :math:`[-1.0, +1.0]`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise overflow, division by zero, and invalid exceptions.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+-----------------------------------------------------+--------------+--------------+-----------------------------------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`                                          | :math:`-0`   | :math:`+0`   | :math:`>0`                                          | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=====================================================+==============+==============+=====================================================+==============+==============+
 * | **erf(x)**          | :math:`-Inf` | :math:`\frac{2}{\sqrt{\pi}}\int_{0}^{x}e^{-t^2}dt`  | :math:`x`                   | :math:`\frac{2}{\sqrt{\pi}}\int_{0}^{x}e^{-t^2}dt`  | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+-----------------------------------------------------+--------------+--------------+-----------------------------------------------------+--------------+--------------+
 *
 */

/* double erf(double x)
 * double erfc(double x)
 *                 x
 *              2      |\
 *     erf(x)  =  ---------  | exp(-t*t)dt
 *            sqrt(pi) \|
 *                 0
 *
 *     erfc(x) =  1-erf(x)
 *  Note that
 *        erf(-x) = -erf(x)
 *        erfc(-x) = 2 - erfc(x)
 *
 * Method:
 *    1. For |x| in [0, 0.84375]
 *        erf(x)  = x + x*R(x^2)
 *          erfc(x) = 1 - erf(x)           if x in [-.84375,0.25]
 *                  = 0.5 + ((0.5-x)-x*R)  if x in [0.25,0.84375]
 *       where R = P/Q where P is an odd poly of degree 8 and
 *       Q is an odd poly of degree 10.
 *                         -57.90
 *            | R - (erf(x)-x)/x | <= 2
 *
 *
 *       Remark. The formula is derived by noting
 *          erf(x) = (2/sqrt(pi))*(x - x^3/3 + x^5/10 - x^7/42 + ....)
 *       and that
 *          2/sqrt(pi) = 1.128379167095512573896158903121545171688
 *       is close to one. The interval is chosen because the fix
 *       point of erf(x) is near 0.6174 (i.e., erf(x)=x when x is
 *       near 0.6174), and by some experiment, 0.84375 is chosen to
 *        guarantee the error is less than one ulp for erf.
 *
 *      2. For |x| in [0.84375,1.25], let s = |x| - 1, and
 *         c = 0.84506291151 rounded to single (24 bits)
 *             erf(x)  = sign(x) * (c  + P1(s)/Q1(s))
 *             erfc(x) = (1-c)  - P1(s)/Q1(s) if x > 0
 *              1+(c+P1(s)/Q1(s))    if x < 0
 *             |P1/Q1 - (erf(|x|)-c)| <= 2**-59.06
 *       Remark: here we use the Taylor series expansion at x=1.
 *        erf(1+s) = erf(1) + s*Poly(s)
 *             = 0.845.. + P1(s)/Q1(s)
 *       That is, we use rational approximation to approximate
 *            erf(1+s) - (c = (single)0.84506291151)
 *       Note that |P1/Q1|< 0.078 for x in [0.84375,1.25]
 *       where
 *        P1(s) = degree 6 poly in s
 *        Q1(s) = degree 6 poly in s
 *
 *      3. For x in [1.25,1/0.35(~2.857143)],
 *             erfc(x) = (1/x)*exp(-x*x-0.5625+R1/S1)
 *             erf(x)  = 1 - erfc(x)
 *       where
 *        R1(z) = degree 7 poly in z, (z=1/x^2)
 *        S1(z) = degree 8 poly in z
 *
 *      4. For x in [1/0.35,28]
 *             erfc(x) = (1/x)*exp(-x*x-0.5625+R2/S2) if x > 0
 *            = 2.0 - (1/x)*exp(-x*x-0.5625+R2/S2) if -6<x<0
 *            = 2.0 - tiny        (if x <= -6)
 *             erf(x)  = sign(x)*(1.0 - erfc(x)) if x < 6, else
 *             erf(x)  = sign(x)*(1.0 - tiny)
 *       where
 *        R2(z) = degree 6 poly in z, (z=1/x^2)
 *        S2(z) = degree 7 poly in z
 *
 *      Note1:
 *       To compute exp(-x*x-0.5625+R/S), let s be a single
 *       precision number and s := x; then
 *        -x*x = -s*s + (s-x)*(s+x)
 *            exp(-x*x-0.5626+R/S) =
 *            exp(-s*s-0.5625)*exp((s-x)*(s+x)+R/S);
 *      Note2:
 *       Here 4 and 5 make use of the asymptotic series
 *              exp(-x*x)
 *        erfc(x) ~ ---------- * ( 1 + Poly(1/x^2) )
 *              x*sqrt(pi)
 *       We use rational approximation to approximate
 *          g(s)=f(1/x^2) = log(erfc(x)*x) - x*x + 0.5625
 *       Here is the error bound for R1/S1 and R2/S2
 *          |R1/S1 - f(x)|  < 2**(-62.57)
 *          |R2/S2 - f(x)|  < 2**(-61.52)
 *
 *      5. For inf > x >= 28
 *             erf(x)  = sign(x) *(1 - tiny)  (raise inexact)
 *             erfc(x) = tiny*tiny (raise underflow) if x > 0
 *            = 2 - tiny if x<0
 *
 *      7. Special case:
 *             erf(0)  = 0, erf(inf)  = 1, erf(-inf) = -1,
 *             erfc(0) = 1, erfc(inf) = 0, erfc(-inf) = 2,
 *           erfc/erf(NaN) is NaN
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/errorfunctiond.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
erx  =  8.45062911510467529297e-01, /* 0x3FEB0AC1, 0x60000000 */
efx  =  1.28379167095512586316e-01, /* 0x3FC06EBA, 0x8214DB69 */
efx8 =  1.02703333676410069053e+00; /* 0x3FF06EBA, 0x8214DB69 */

double erf(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, ix;
    double R, S, P, Q, s, z, r;
    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x7ff00000) {
        if (isnan(x)) {         /* erf(nan) = nan */
            return x + x;
        } else if (hx > 0) {    /* erf(+inf) = +1 */
            return 1.0;
        } else {                /* erf(-inf) = -1 */
            return -1.0;
        }
    }

    if (ix < 0x3feb0000) {                /* |x|<0.84375 */
        if (ix < 0x3e300000) {            /* |x|<2**-28 */
            if (ix < 0x00800000) {
                return 0.125 * (8.0 * x + efx8 * x);    /*avoid underflow */
            }

            return x + efx * x;
        }

        return x + x * __erf_y(x);
    }

    if (ix < 0x3ff40000) {                /* 0.84375 <= |x| < 1.25 */
        s = fabs(x) - one;
        P = __erf_P(s);
        Q = __erf_Q(s);

        if (hx >= 0) {
            return erx + P / Q;
        } else {
            return -erx - P / Q;
        }
    }

    if (ix >= 0x40180000) {               /* inf>|x|>=6 */
        if (hx >= 0) {
            return __raise_inexact(one);
        } else {
            return -__raise_inexact(one);
        }
    }

    x = fabs(x);
    s = one / (x * x);

    if (ix < 0x4006DB6E) {                /* |x| < 1/0.35 */
        R = __erf_Ra(s);
        S = __erf_Sa(s);
    } else {                              /* |x| >= 1/0.35 */
        R = __erf_Rb(s);
        S = __erf_Sb(s);
    }

    z  = x;
    SET_LOW_WORD(z, 0);
    r  =  exp(-z * z - 0.5625) * exp((z - x) * (z + x) + R / S);

    if (hx >= 0) {
        return one - r / x;
    } else {
        return  r / x - one;
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double erfl(long double x)
{
    return (long double) erf((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
