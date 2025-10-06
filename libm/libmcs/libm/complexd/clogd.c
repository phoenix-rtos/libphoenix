/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex natural logarithm function,
 * that is :math:`ln(z)`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float complex clogf(float z);
 *     double complex clog(double z);
 *     long double complex clogl(long double z);
 *
 * Description
 * ===========
 *
 * ``clog`` computes the complex natural logarithm of :math:`z`, with a branch
 * cut along the negative real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    clog(z) \approx ln(z)
 *
 * Returns
 * =======
 *
 * ``clog`` returns the complex natural logarithm of the input value in the
 * output range of a strip mathematically unbounded along the real axis and in
 * the interval :math:`[-\pi i, \pi i]` along the imaginary axis.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex clog(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double p, rr;

    rr = cabs(z);
    p = log(rr);
    rr = atan2(cimag(z), creal(z));
    /* w = p + rr * I; */
    w = CMPLX(p, rr);
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex clogl(long double complex z)
{
    return (long double complex) clog((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
