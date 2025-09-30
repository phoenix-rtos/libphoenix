/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex exponential function, that
 * is :math:`e` powered by :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float complex cexpf(float z);
 *     double complex cexp(double z);
 *     long double complex cexpl(long double z);
 *
 * Description
 * ===========
 *
 * ``cexp`` computes :math:`e` powered by the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cexp(z) \approx e^z
 *
 * Returns
 * =======
 *
 * ``cexp`` returns :math:`e` powered by :math:`z`.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex cexp(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double r, x, y;

    x = creal(z);
    y = cimag(z);
    r = exp(x);
    /* w = r * cos(y) + r * sin(y) * I; */
    w = CMPLX(r * cos(y), r * sin(y));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex cexpl(long double complex z)
{
    return (long double complex) cexp((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
