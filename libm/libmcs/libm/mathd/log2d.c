/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the base :math:`2` logarithm.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float log2f(float x);
 *     double log2(double x);
 *     long double logl(long double x);
 *
 * Description
 * ===========
 *
 * ``log2`` computes the base :math:`2` logarithm of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    log2(x) \approx log_{2}(x)
 *
 * Returns
 * =======
 *
 * ``log2`` returns the base :math:`2` logarithm of :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is negative.
 *
 * Raise ``divide by zero`` exception when the input value is zero.
 *
 * Output map
 * ==========
 *
 * +---------------------+---------------+---------------+---------------+---------------+---------------------+---------------+---------------------+---------------+---------------+
 * | **x**               | :math:`-Inf`  | :math:`<0`    | :math:`-0`    | :math:`+0`    | :math:`]0,1[`       | :math:`1`     | :math:`>1`          | :math:`+Inf`  | :math:`NaN`   |
 * +=====================+===============+===============+===============+===============+=====================+===============+=====================+===============+===============+
 * | **log2(x)**         | :math:`qNaN`  | :math:`qNaN`  | :math:`-Inf`                  | :math:`log_{2}(x)`  | :math:`+0`    | :math:`log_{2}(x)`  | :math:`+Inf`  | :math:`qNaN`  |
 * +---------------------+---------------+---------------+---------------+---------------+---------------------+---------------+---------------------+---------------+---------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/log1pmfd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
two54      =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
ivln2hi    =  1.44269504072144627571e+00, /* 0x3ff71547, 0x65200000 */
ivln2lo    =  1.67517131648865118353e-10; /* 0x3de705fc, 0x2eefa200 */

static const double zero   =  0.0;

double log2(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double f, hfsq, hi, lo, r, val_hi, val_lo, w, y;
    int32_t i, k, hx;
    uint32_t lx;

    EXTRACT_WORDS(hx, lx, x);

    k = 0;

    if (hx < 0x00100000) {              /* x < 2**-1022  */
        if (((hx & 0x7fffffff) | lx) == 0) {
            return __raise_div_by_zero(-1.0);     /* log(+-0)=-inf */
        }

        if (hx < 0) {
            if (isnan(x)) {
                return x + x;
            } else {
                return __raise_invalid();   /* log(-#) = NaN */
            }
        }

        k -= 54;
        x *= two54;                     /* subnormal number, scale up x */
        GET_HIGH_WORD(hx, x);
    }

    if (hx >= 0x7ff00000) {             /* x = NaN/+-Inf */
        return x + x;
    }

    if (hx == 0x3ff00000 && lx == 0) {  /* log(1) = +0 */
        return zero;
    }

    k += (hx >> 20) - 1023;
    hx &= 0x000fffff;
    i = (hx + 0x95f64) & 0x100000;
    SET_HIGH_WORD(x, hx | (i ^ 0x3ff00000)); /* normalize x or x/2 */
    k += (i >> 20);
    y = (double)k;
    f = x - 1.0;
    hfsq = 0.5 * f * f;
    r = __log1pmf(f);

    /*
     * f-hfsq must (for args near 1) be evaluated in extra precision
     * to avoid a large cancellation when x is near sqrt(2) or 1/sqrt(2).
     * This is fairly efficient since f-hfsq only depends on f, so can
     * be evaluated in parallel with R.  Not combining hfsq with R also
     * keeps R small (though not as small as a true `lo' term would be),
     * so that extra precision is not needed for terms involving R.
     *
     * Compiler bugs involving extra precision used to break Dekker's
     * theorem for spitting f-hfsq as hi+lo, unless double_t was used
     * or the multi-precision calculations were avoided when double_t
     * has extra precision.  These problems are now automatically
     * avoided as a side effect of the optimization of combining the
     * Dekker splitting step with the clear-low-bits step.
     *
     * y must (for args near sqrt(2) and 1/sqrt(2)) be added in extra
     * precision to avoid a very large cancellation when x is very near
     * these values.  Unlike the above cancellations, this problem is
     * specific to base 2.  It is strange that adding +-1 is so much
     * harder than adding +-ln2 or +-log10_2.
     *
     * This uses Dekker's theorem to normalize y+val_hi, so the
     * compiler bugs are back in some configurations, sigh.  And I
     * don't want to used double_t to avoid them, since that gives a
     * pessimization and the support for avoiding the pessimization
     * is not yet available.
     *
     * The multi-precision calculations for the multiplications are
     * routine.
     */
    hi = f - hfsq;
    SET_LOW_WORD(hi, 0);
    lo = (f - hi) - hfsq + r;
    val_hi = hi * ivln2hi;
    val_lo = (lo + hi) * ivln2lo + lo * ivln2hi;

    w = y + val_hi;
    val_lo += (y - w) + val_hi;
    val_hi = w;

    return val_lo + val_hi;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double log2l(long double x)
{
    return (long double) log2((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
