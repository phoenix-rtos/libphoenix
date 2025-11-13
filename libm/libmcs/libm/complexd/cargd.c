/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

/**
 *
 * This family of functions implements the argument of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float cargf(float complex z);
 *     double carg(double complex z);
 *     long double cargl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``carg`` computes the argument (also called phase angle) of the input value,
 * with a branch cut along the negative real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    carg(z) \approx \varphi (z) = \tan^{-1} \frac{\Im(z)}{\Re(z)}
 *
 * Returns
 * =======
 *
 * ``carg`` returns the argument of the input value in the interval
 * :math:`[-\pi, \pi]`.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double carg(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return atan2(cimag(z), creal(z));
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double cargl(long double complex z)
{
    return (long double) carg((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
