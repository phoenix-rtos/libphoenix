/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex cpowf(float complex x, float complex y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float realz, imagz, result, theta, absx, argx;

    realz = crealf(y);
    imagz = cimagf(y);
    absx = cabsf(x);

    if (absx == 0.0f) {
        return CMPLXF(0.0f, 0.0f);
    }

    argx = cargf(x);
    result = powf(absx, realz);
    theta = realz * argx;

    if (imagz != 0.0f) {
        result = result * expf(-imagz * argx);
        theta = theta + imagz * logf(absx);
    }

    /*w = result * cosf(theta) + (result * sinf(theta)) * I; */
    w = CMPLXF(result * cosf(theta), result * sinf(theta));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex cpow(double complex x, double complex y)
{
    return (double complex) cpowf((float complex) x, (float complex) y);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
