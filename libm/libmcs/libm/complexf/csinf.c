/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>
#include "internal/ctrigf.h"

float complex csinf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float ch, sh;

    __ccoshsinhf(cimagf(z), &ch, &sh);
    /* w = sinf(crealf(z)) * ch + (cosf(crealf(z)) * sh) * I; */
    w = CMPLXF(sinf(crealf(z)) * ch, cosf(crealf(z)) * sh);
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex csin(double complex z)
{
    return (double complex) csinf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
