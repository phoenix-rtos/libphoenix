/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic arc cosine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex cacoshf(float complex z);
 *     double complex cacosh(double complex z);
 *     long double complex cacoshl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``cacosh`` computes the complex hyperbolic inverse cosine (*arc cosine*) of
 * the input value, with a branch cut at values less than :math:`+1` along the
 * real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cacosh(z) \approx cosh^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``cacosh`` returns the complex hyperbolic inverse cosine of the input value
 * in the output range of a half-strip of non-negative values along the real
 * axis and in the interval :math:`[-\pi i, +\pi i]` along the imaginary axis.
 *
 */

#include <complex.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex cacosh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;

    w = clog(z + csqrt(z + 1) * csqrt(z - 1));

    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex cacoshl(long double complex z)
{
    return (long double complex) cacosh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
