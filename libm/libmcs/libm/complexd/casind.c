/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex arc sine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex casinf(float complex z);
 *     double complex casin(double complex z);
 *     long double complex casinl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``casin`` computes the complex inverse sine (*arc sine*) of the input value,
 * with branch cuts outside the interval :math:`[-1, +1]` along the real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    casin(z) \approx sin^{-1}(z)
 *
 * Returns
 * =======
 *
 * ``casin`` returns the complex inverse sine of the input value in the output
 * range of a strip mathematically unbounded along the imaginary axis and in
 * the interval :math:`[-\frac{\pi}{2}, \frac{\pi}{2}]` radians along the real
 * axis.
 *
 */

#include <complex.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex casin(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double complex ct, zz, z2;
    double x, y;

    x = creal(z);
    y = cimag(z);

    ct = CMPLX(-y, x);
    /* zz = (x - y) * (x + y) + (2.0 * x * y) * I; */
    zz = CMPLX((x - y) * (x + y), 2.0 * x * y);

    /* zz = 1.0 - creal(zz) - cimag(zz) * I; */
    zz = CMPLX(1.0 - creal(zz), -cimag(zz));
    z2 = csqrt(zz);

    zz = ct + z2;
    zz = clog(zz);
    /* w = zz * (-1.0 * I); */
    w = CMPLX(cimag(zz), -creal(zz));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex casinl(long double complex z)
{
    return (long double complex) casin((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
