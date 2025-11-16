/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic arc tangent of
 * :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex catanhf(float complex z);
 *     double complex catanh(double complex z);
 *     long double complex catanhl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``catanh`` computes the complex hyperbolic inverse tangent (*arc tangent*)
 * of the input value, with branch cuts outside the interval :math:`[-1, +1]`
 * along the real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    catanh(z) \approx tanh^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``catanh`` returns the complex hyperbolic inverse tangent of the input value
 * in the output range of a strip mathematically unbounded along the real axis
 * and in the interval :math:`[-\frac{\pi}{2}i, \frac{\pi}{2}i]` along the
 * imaginary axis.
 *
 */

#include <complex.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex catanh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double complex tmp;

    /* w = -1.0 * I * catan(z * I) */
    tmp = CMPLX(-cimag(z), creal(z));
    tmp = catan(tmp);
    w = CMPLX(cimag(tmp), -creal(tmp));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex catanhl(long double complex z)
{
    return (long double complex) catanh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
