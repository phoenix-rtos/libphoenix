/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

#include <complex.h>
#include <math.h>

float cargf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return atan2f(cimagf(z), crealf(z));
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double carg(double complex z)
{
    return (double) cargf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
