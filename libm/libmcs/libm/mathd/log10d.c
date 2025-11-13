/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the base :math:`10` logarithm.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float log10f(float x);
 *     double log10(double x);
 *     long double logl(long double x);
 *
 * Description
 * ===========
 *
 * ``log10`` computes the base :math:`10` logarithm of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    log10(x) \approx log_{10}(x)
 *
 * Returns
 * =======
 *
 * ``log10`` returns the base :math:`10` logarithm of :math:`x`.
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
 * | **log10(x)**        | :math:`qNaN`  | :math:`qNaN`  | :math:`-Inf`                  | :math:`log_{10}(x)` | :math:`+0`    | :math:`log_{10}(x)` | :math:`+Inf`  | :math:`qNaN`  |
 * +---------------------+---------------+---------------+---------------+---------------+---------------------+---------------+---------------------+---------------+---------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/log1pmfd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
two54      =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
ivln10hi   =  4.34294481878168880939e-01, /* 0x3FDBCB7B, 0x15200000 */
ivln10lo   =  2.50829467116452752298e-11, /* 0x3DBB9438, 0xCA9AADD5 */
log10_2hi  =  3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo  =  3.69423907715893078616e-13; /* 0x3D59FEF3, 0x11F12B36 */

static const double zero  =  0.0;

double log10(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double f, hfsq, hi, lo, r, val_hi, val_lo, w, y, y2;
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

    hi = f - hfsq;
    SET_LOW_WORD(hi, 0);
    lo = (f - hi) - hfsq + r;
    val_hi = hi * ivln10hi;
    y2 = y * log10_2hi;
    val_lo = y * log10_2lo + (lo + hi) * ivln10lo + lo * ivln10hi;

    /*
     * Extra precision in for adding y*log10_2hi is not strictly needed
     * since there is no very large cancellation near x = sqrt(2) or
     * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
     * with some parallelism and it reduces the error for many args.
     */
    w = y2 + val_hi;
    val_lo += (y2 - w) + val_hi;
    val_hi = w;

    return val_lo + val_hi;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double log10l(long double x)
{
    return (long double) log10((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
