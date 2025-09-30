/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic cosine of
 * :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex ccoshf(float complex z);
 *     double complex ccosh(double complex z);
 *     long double complex ccoshl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``ccosh`` computes the complex hyperbolic cosine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ccosh(z) \approx cosh(z)
 *
 * Returns
 * =======
 *
 * ``ccosh`` returns the complex hyperbolic cosine of the input value.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex ccosh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double x, y;

    x = creal(z);
    y = cimag(z);
    /* w = cosh(x) * cos(y) + (sinh(x) * sin(y)) * I; */
    w = CMPLX(cosh(x) * cos(y), sinh(x) * sin(y));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex ccoshl(long double complex z)
{
    return (long double complex) ccosh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
