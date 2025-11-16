/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex arc cosine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex cacosf(float complex z);
 *     double complex cacos(double complex z);
 *     long double complex cacosl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``cacos`` computes the complex inverse cosine (*arc cosine*) of the input
 * value, with branch cuts outside the interval :math:`[-1, +1]` along the real
 * axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cacos(z) \approx cos^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``cacos`` returns the complex inverse cosine of the input value in the
 * output range of a strip mathematically unbounded along the imaginary axis
 * and in the interval :math:`[0, \pi]` radians along the real axis.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex cacos(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double complex tmp0;
    double tmp1;

    tmp0 = casin(z);
    tmp1 = M_PI_2 - creal(tmp0);
    /*  w = tmp1 - (cimag(tmp0) * I); */
    w = CMPLX(tmp1, -cimag(tmp0));

    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex cacosl(long double complex z)
{
    return (long double complex) cacos((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
