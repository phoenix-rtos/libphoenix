/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex hyperbolic arc sine of
 * :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex casinhf(float complex z);
 *     double complex casinh(double complex z);
 *     long double complex casinhl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``casinh`` computes the complex hyperbolic inverse sine (*arc sine*) of the
 * input value, with branch cuts outside the interval :math:`[-i, +i]` along
 * the imaginary axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    casinh(z) \approx sinh^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``casinh`` returns the complex hyperbolic inverse sine of the input value in
 * the output range of a strip mathematically unbounded along the real axis and
 * in the interval :math:`[-\frac{\pi}{2}i, \frac{\pi}{2}i]` along the
 * imaginary axis.
 *
 */

#include <complex.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex casinh(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double complex tmp;

    /* w = -1.0 * I * casin(z * I); */
    tmp = CMPLX(-cimag(z), creal(z));
    tmp = casin(tmp);
    w = CMPLX(cimag(tmp), -creal(tmp));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex casinhl(long double complex z)
{
    return (long double complex) casinh((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
