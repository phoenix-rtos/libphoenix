/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

/**
 *
 * This family of functions returns the imaginary part of :math:`z` as a real.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float cimagf(float complex z);
 *     double cimag(double complex z);
 *     long double cimagl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``cimag`` computes the imaginary part of :math:`z` as a real.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cimag(z) = \Im(z)
 *
 * Returns
 * =======
 *
 * ``cimag`` returns the imaginary part of :math:`z` as a real.
 *
 */

#include <complex.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double cimag(double complex z)
{
    double_complex w;
    w.z = z;

    return (IMAG_PART(w));
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double cimagl(long double complex z)
{
    return (long double) cimag((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
