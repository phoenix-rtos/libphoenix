/* SPDX-License-Identifier: NetBSD */

/**
 *
 * This family of functions implements the complex square root of :math:`z`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float complex csqrtf(float complex z);
 *     double complex csqrt(double complex z);
 *     long double complex csqrtl(long double complex z);
 *
 * Description
 * ===========
 *
 * ``csqrt`` computes the complex square root of the input value, with a branch
 * cut along the negative real axis.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    csqrt(z) \approx \sqrt{z}
 *
 * Returns
 * =======
 *
 * ``csqrt`` returns the complex square root of the input value, in the range
 * of the right halfplane (including the imaginary axis).
 *
 */

#include <complex.h>
#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double complex csqrt(double complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double complex w;
    double x, y, r, t, scale;

    x = creal(z);
    y = cimag(z);

    if (y == 0.0) {
        if (x == 0.0) {
            w = CMPLX(0.0, y);
        } else {
            r = fabs(x);
            r = sqrt(r);

            if (x < 0.0) {
                w = CMPLX(0.0, r);
            } else {
                w = CMPLX(r, y);
            }
        }

        return w;
    }

    if (x == 0.0) {
        r = fabs(y);
        r = sqrt(0.5 * r);

        if (y > 0) {
            w = CMPLX(r, r);
        } else {
            w = CMPLX(r, -r);
        }

        return w;
    }

    /* Rescale to avoid internal overflow or underflow.  */
    if ((fabs(x) > 4.0) || (fabs(y) > 4.0)) {
        x *= 0.25;
        y *= 0.25;
        scale = 2.0;
    } else {
        x *= 1.8014398509481984e16;  /* 2^54 */
        y *= 1.8014398509481984e16;
        scale = 7.450580596923828125e-9; /* 2^-27 */
    }

    w = CMPLX(x, y);
    r = cabs(w);

    if (x > 0) {
        t = sqrt(0.5 * r + 0.5 * x);
        r = scale * fabs((0.5 * y) / t);
        t *= scale;
    } else {
        r = sqrt(0.5 * r - 0.5 * x);
        t = scale * fabs((0.5 * y) / r);
        r *= scale;
    }

    if (y < 0) {
        w = CMPLX(t, -r);
    } else {
        w = CMPLX(t, r);
    }

    return w;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double complex csqrtl(long double complex z)
{
    return (long double complex) csqrt((double complex) z);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
