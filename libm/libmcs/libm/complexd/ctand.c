/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex tangent of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex ctanf(float complex z);
 *     double complex ctan(double complex z);
 *     long double complex ctanl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``ctan`` computes the complex tangent of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ctan(z) \approx tan(z)
 *
 * Returns
 * =======
 *
 * ``ctan`` returns the complex tangent of the input value.
 *
 */

#include <complex.h>
#include <math.h>
#include "internal/ctrigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

/* Taylor series expansion for cosh(2y) - cos(2x) */

static inline double __ctans(double complex z)
{
    double f, x, x2, y, y2, rn, t;
    double d;

    x = fabs(2.0 * creal(z));
    y = fabs(2.0 * cimag(z));

    x = __redupi(x);

    x = x * x;
    y = y * y;
    x2 = 1.0;
    y2 = 1.0;
    f = 1.0;
    rn = 0.0;
    d = 0.0;

    do {
        rn += 1.0;
        f *= rn;
        rn += 1.0;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 + x2;
        t /= f;
        d += t;

        rn += 1.0;
        f *= rn;
        rn += 1.0;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 - x2;
        t /= f;
        d += t;
    } while (fabs(t / d) > MACHEP);

    return d;
}

double complex ctan(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double d;

    d = cos(2.0 * creal(z)) + cosh(2.0 * cimag(z));

    if (fabs(d) < 0.25) {
        d = __ctans(z);
    }

    if (d == 0.0) {
        w = CMPLX(HUGE_VAL, HUGE_VAL);
        return w;
    }

    /* w = sin(2.0 * creal(z)) / d + (sinh(2.0 * cimag(z)) / d) * I; */
    w = CMPLX(sin(2.0 * creal(z)) / d, sinh(2.0 * cimag(z)) / d);
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex ctanl(long double complex z)
{
    return (long double complex) ctan((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
