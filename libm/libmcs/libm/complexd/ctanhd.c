/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic tangent of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex ctanhf(float complex z);
 *     double complex ctanh(double complex z);
 *     long double complex ctanhl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``ctanh`` computes the complex hyperbolic tangent of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ctanh(z) \approx tanh(z)
 *
 * Returns
 * =======
 *
 * ``ctanh`` returns the complex hyperbolic tangent of the input value.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex ctanh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double x, y, d;

    x = creal(z);
    y = cimag(z);
    d = cosh(2.0 * x) + cos(2.0 * y);
    /* w = sinh(2.0 * x) / d  + (sin(2.0 * y) / d) * I; */
    w = CMPLX(sinh(2.0 * x) / d, sin(2.0 * y) / d);

    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex ctanhl(long double complex z)
{
    return (long double complex) ctanh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
