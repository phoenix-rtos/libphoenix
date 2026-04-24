/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex sine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex csinf(float complex z);
 *     double complex csin(double complex z);
 *     long double complex csinl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``csin`` computes the complex sine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    csin(z) \approx sin(z)
 *
 * Returns
 * =======
 *
 * ``csin`` returns the complex sine of the input value.
 *
 */

#include <complex.h>
#include <math.h>
#include "internal/ctrigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex csin(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double ch, sh;

    __ccoshsinh(cimag(z), &ch, &sh);
    /* w = sin(creal(z)) * ch + (cos(creal(z)) * sh) * I; */
    w = CMPLX(sin(creal(z)) * ch, cos(creal(z)) * sh);
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex csinl(long double complex z)
{
    return (long double complex) csin((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
