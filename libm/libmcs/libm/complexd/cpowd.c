/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the value of complex :math:`x` raised to
 * the power of complex :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float complex cpowf(float complex x, float complex y);
 *     double complex cpow(double complex x, double complex y);
 *     long double complex cpowl(long double complex x, long double complex y);
 *
 * Description
 * ===========
 *
 * ``cpow`` computes the value of complex :math:`x` raised to the power of
 * complex :math:`y`, with a branch cut for the first parameter along the
 * negative real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cpow(x, y) \approx x^y
 *
 * Returns
 * =======
 *
 * ``cpow`` returns the value of complex :math:`x` raised to the power of
 * complex :math:`y`.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex cpow(double complex x, double complex y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double realz, imagz, result, theta, absx, argx;

    realz = creal(y);
    imagz = cimag(y);
    absx = cabs(x);

    if (absx == 0.0) {
        return CMPLX(0.0, 0.0);
    }

    argx = carg(x);
    result = pow(absx, realz);
    theta = realz * argx;

    if (imagz != 0.0) {
        result = result * exp(-imagz * argx);
        theta = theta + imagz * log(absx);
    }

    /* w = result * cos(theta) + (result * sin(theta)) * I; */
    w = CMPLX(result * cos(theta), result * sin(theta));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex cpowl(long double complex x, long double complex y)
{
    return (long double complex) cpow((double complex) x, (double complex) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
