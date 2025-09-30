/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex cosine of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <complex.h>
 *     float complex ccosf(float complex z);
 *     double complex ccos(double complex z);
 *     long double complex ccosl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``ccos`` computes the complex cosine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ccos(z) \approx cos(z)
 *
 * Returns
 * =======
 *
 * ``ccos`` returns the complex cosine of the input value.
 *
 */

#include <complex.h>
#include <math.h>
#include "internal/ctrigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex ccos(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double ch, sh;

    __ccoshsinh(cimag(z), &ch, &sh);
    /* w = cos(creal(z)) * ch - (sin(creal(z)) * sh) * I; */
    w = CMPLX(cos(creal(z)) * ch, -(sin(creal(z)) * sh));
    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex ccosl(long double complex z)
{
    return (long double complex) ccos((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
