/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

/**
 *
 * This family of functions evaluates the complex conjugate of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex conjf(float complex z);
 *     double complex conj(double complex z);
 *     long double complex conjl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``conj`` computes the complex conjugate of :math:`z`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    conj(z) = \Re(z) - \Im(z) \cdot i
 *
 * Returns
 * =======
 *
 * ``conj`` returns the complex conjugate of :math:`z`.
 *
 */

/*
FUNCTION
        <<conj>>, <<conjf>>---complex conjugate

INDEX
        conj
INDEX
        conjf

SYNOPSIS
       #include <complex.h>
       double complex conj(double complex <[z]>);
       float complex conjf(float complex <[z]>);


DESCRIPTION
        These functions compute the complex conjugate of <[z]>,
        by reversing the sign of its imaginary part.

        <<conjf>> is identical to <<conj>>, except that it performs
        its calculations on <<floats complex>>.

RETURNS
        The conj functions return the complex conjugate value.

PORTABILITY
        <<conj>> and <<conjf>> are ISO C99

QUICKREF
        <<conj>> and <<conjf>> are ISO C99

*/

#include <complex.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex conj(double complex z)
{
    double_complex w = { .z = z };

    IMAG_PART(w) = -IMAG_PART(w);

    return (w.z);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex conjl(long double complex z)
{
    return (long double complex) conj((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
