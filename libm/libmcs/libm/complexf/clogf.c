/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex clogf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float p, rr;

    rr = cabsf(z);
    p = logf(rr);
    rr = atan2f(cimagf(z), crealf(z));
    /* w = p + rr * I; */
    w = CMPLXF(p, rr);
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex clog(double complex z)
{
    return (double complex) clogf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
