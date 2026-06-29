/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex arc tangent of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex catanf(float complex z);
 *     double complex catan(double complex z);
 *     long double complex catanl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``catan`` computes the complex inverse tangent (*arc tangent*) of the input
 * value, with branch cuts outside the interval :math:`[-i, +i]` along the
 * imaginary axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    catan(z) \approx tan^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``catan`` returns the complex inverse tangent of the input value in the
 * output range of a strip mathematically unbounded along the imaginary axis
 * and in the interval :math:`[-\frac{\pi}{2}, \frac{\pi}{2}]` radians along
 * the real axis.
 *
 */

#include <complex.h>
#include <math.h>
#include "internal/ctrigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex catan(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double a, t, x, x2, y, tmp;

    x = creal(z);
    y = cimag(z);

    if ((x == 0.0) && (y > 1.0)) {
        goto ovrf;
    }

    x2 = x * x;
    a = 1.0 - x2 - (y * y);

    t = 0.5 * atan2(2.0 * x, a);
    tmp = __redupi(t);

    t = y - 1.0;
    a = x2 + (t * t);

    t = y + 1.0;
    a = (x2 + (t * t)) / a;
    /* w = tmp + (0.25 * log(a)) * I; */
    w = CMPLX(tmp, 0.25 * log(a));
    return w;

ovrf:
    w = CMPLX(HUGE_VAL, HUGE_VAL);
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex catanl(long double complex z)
{
    return (long double complex) catan((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
