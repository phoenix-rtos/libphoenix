/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

/**
 *
 * This family of functions implements the absolute value ot the complex
 * argument :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float cabsf(float z);
 *     double cabs(double z);
 *     long double cabsl(long double z);
 *
 * Description
 * ===========
 *
 * ``cabs`` computes the absolute value (also called norm, modulus, or
 * magnitude) of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cabs(z) \approx |z|
 *
 * Returns
 * =======
 *
 * ``cabs`` returns the absolute value of :math:`z`.
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double cabs(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return hypot(creal(z), cimag(z));
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double cabsl(long double complex z)
{
    return (long double) cabs((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
