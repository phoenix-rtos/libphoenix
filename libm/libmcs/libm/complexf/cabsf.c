/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

#include <complex.h>
#include <math.h>

float cabsf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return hypotf(crealf(z), cimagf(z));
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double cabs(double complex z)
{
    return (double) cabsf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
