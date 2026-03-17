/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions evaluates the value of the projection of the
 * complex argument :math:`z` onto the Riemann sphere.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex cprojf(float complex z);
 *     double complex cproj(double complex z);
 *     long double complex cprojl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``cproj`` computes the value of the projection of :math:`z` onto the Riemann
 * sphere.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cproj(z) = \left\{\begin{array}{ll} z, & \Re(z) \neq \pm Inf \wedge \Im(z) \neq \pm Inf \\ +Inf \pm 0i, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``cproj`` returns the value of the projection of :math:`z` onto the Riemann
 * sphere.
 *
 */

#include <complex.h>
#include <math.h>

#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

/*
 * cproj(double complex z)
 *
 * These functions return the value of the projection (not stereographic!)
 * onto the Riemann sphere.
 *
 * z projects to z, except that all complex infinities (even those with one
 * infinite part and one NaN part) project to positive infinity on the real axis.
 * If z has an infinite part, then cproj(z) shall be equivalent to:
 *
 * INFINITY + I * copysign(0.0, cimag(z))
 */
double complex cproj(double complex z)
{
    double_complex w = { .z = z };

    if (isinf(REAL_PART(w)) || isinf(IMAG_PART(w))) {
        REAL_PART(w) = INFINITY;
        IMAG_PART(w) = copysign(0.0, cimag(z));
    }

    return (w.z);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex cprojl(long double complex z)
{
    return (long double complex) cproj((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
