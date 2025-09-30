/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>
#include "internal/ctrigf.h"

float complex ccosf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float ch, sh;

    __ccoshsinhf(cimagf(z), &ch, &sh);
    /* w = cosf(crealf(z)) * ch - (sinf(crealf(z)) * sh) * I; */
    w = CMPLXF(cosf(crealf(z)) * ch, -(sinf(crealf(z)) * sh));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex ccos(double complex z)
{
    return (double complex) ccosf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
