/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

/**
 *
 * This family of functions returns the real part of :math:`z` as a real.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float crealf(float complex z);
 *     double creal(double complex z);
 *     long double creall(long double complex z);
 *
 * Description
 * ===========
 *
 * ``creal`` computes the real part of :math:`z` as a real.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    creal(z) = \Re(z)
 *
 * Returns
 * =======
 *
 * ``creal`` returns the real part of :math:`z` as a real.
 *
 */

#include <complex.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double creal(double complex z)
{
    double_complex w;
    w.z = z;

    return (REAL_PART(w));
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double creall(long double complex z)
{
    return (long double) creal((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
