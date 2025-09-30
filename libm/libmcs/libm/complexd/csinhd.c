/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic sine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex csinhf(float complex z);
 *     double complex csinh(double complex z);
 *     long double complex csinhl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``csinh`` computes the complex hyperbolic sine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    csinh(z) \approx sinh(z)
 *
 * Returns
 * =======
 *
 * ``csinh`` returns the complex hyperbolic sine of the input value.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex csinh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double x, y;

    x = creal(z);
    y = cimag(z);
    /* w = sinh(x) * cos(y) + (cosh(x) * sin(y)) * I; */
    w = CMPLX(sinh(x) * cos(y), cosh(x) * sin(y));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex csinhl(long double complex z)
{
    return (long double complex) csinh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
