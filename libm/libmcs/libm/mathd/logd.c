/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the natural logarithm.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float logf(float x);
 *     double log(double x);
 *     long double logl(long double x);
 *
 * Description
 * ===========
 *
 * ``log`` computes the natural logarithm of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    log(x) \approx ln(x)
 *
 * Returns
 * =======
 *
 * ``log`` returns the natural logarithm of :math:`x`.
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
 * +---------------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 * | **x**               | :math:`-Inf`  | :math:`<0`    | :math:`-0`    | :math:`+0`    | :math:`]0,1[` | :math:`1`     | :math:`>1`    | :math:`+Inf`  | :math:`NaN`   |
 * +=====================+===============+===============+===============+===============+===============+===============+===============+===============+===============+
 * | **log(x)**          | :math:`qNaN`  | :math:`qNaN`  | :math:`-Inf`                  | :math:`ln(x)` | :math:`+0`    | :math:`ln(x)` | :math:`+Inf`  | :math:`qNaN`  |
 * +---------------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
ln2_hi  =  6.93147180369123816490e-01,  /* 3fe62e42 fee00000 */
ln2_lo  =  1.90821492927058770002e-10,  /* 3dea39ef 35793c76 */
two54   =  1.80143985094819840000e+16,  /* 43500000 00000000 */
Lg1     =  6.666666666666735130e-01,    /* 3FE55555 55555593 */
Lg2     =  3.999999999940941908e-01,    /* 3FD99999 9997FA04 */
Lg3     =  2.857142874366239149e-01,    /* 3FD24924 94229359 */
Lg4     =  2.222219843214978396e-01,    /* 3FCC71C5 1D8E78AF */
Lg5     =  1.818357216161805012e-01,    /* 3FC74664 96CB03DE */
Lg6     =  1.531383769920937332e-01,    /* 3FC39A09 D078C69F */
Lg7     =  1.479819860511658591e-01;    /* 3FC2F112 DF3E5244 */

static const double zero  =  0.0;

double log(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double hfsq, f, s, z, R, w, t1, t2, dk;
    int32_t k, hx, i, j;
    uint32_t lx;

    EXTRACT_WORDS(hx, lx, x);

    k = 0;

    if (hx < 0x00100000) {                      /* x < 2**-1022  */
        if (((hx & 0x7fffffff) | lx) == 0) {
            return __raise_div_by_zero(-1.0);   /* log(+-0)=-inf */
        }

        if (hx < 0) {
            if (isnan(x)) {
                return x + x;
            } else {
                return __raise_invalid();   /* log(-#) = NaN */
            }
        }

        k -= 54;
        x *= two54;                      /* subnormal number, scale up x */
        GET_HIGH_WORD(hx, x);
    }

    if (hx >= 0x7ff00000) {                     /* x = NaN/+-Inf */
        return x + x;
    }

    k += (hx >> 20) - 1023;
    hx &= 0x000fffff;
    i = (hx + 0x95f64) & 0x100000;
    SET_HIGH_WORD(x, hx | (i ^ 0x3ff00000)); /* normalize x or x/2 */
    k += (i >> 20);
    f = x - 1.0;

    if ((0x000fffff & (2 + hx)) < 3) {       /* |f| < 2**-20 */
        if (f == zero) {
            if (k == 0) {
                return zero;
            }  else {
                dk = (double)k;
                return dk * ln2_hi + dk * ln2_lo;
            }
        }

        R = f * f * (0.5 - 0.33333333333333333 * f);

        if (k == 0) {
            return f - R;
        } else {
            dk = (double)k;
            return dk * ln2_hi - ((R - dk * ln2_lo) - f);
        }
    }

    s = f / (2.0 + f);
    dk = (double)k;
    z = s * s;
    i = hx - 0x6147a;
    w = z * z;
    j = 0x6b851 - hx;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    i |= j;
    R = t2 + t1;

    if (i > 0) {
        hfsq = 0.5 * f * f;

        if (k == 0) {
            return f - (hfsq - s * (hfsq + R));
        } else {
            return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
        }
    } else {
        if (k == 0) {
            return f - s * (f - R);
        } else {
            return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
        }
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double logl(long double x)
{
    return (long double) log((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
