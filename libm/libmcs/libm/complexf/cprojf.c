/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

#include "../common/tools.h"

float complex cprojf(float complex z)
{
    float_complex w = { .z = z };

    if (isinf(REAL_PART(w)) || isinf(IMAG_PART(w))) {
        REAL_PART(w) = INFINITY;
        IMAG_PART(w) = copysignf(0.0, cimagf(z));
    }

    return (w.z);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex cproj(double complex z)
{
    return (double complex) cprojf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
