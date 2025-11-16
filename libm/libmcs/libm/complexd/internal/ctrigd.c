/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

#include "ctrigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

/* calculate cosh and sinh */

void __ccoshsinh(double x, double *c, double *s)
{
    double e, ei;

    if (fabs(x) <= 0.5) {
        *c = cosh(x);
        *s = sinh(x);
    } else {
        e = exp(x);
        if (__fpclassifyd(e) != FP_ZERO) {
            ei = 0.5 / e;
        } else {
            ei = HUGE_VAL;
        }
        e = 0.5 * e;
        *s = e - ei;
        *c = e + ei;
    }
}

/* Program to subtract nearest integer multiple of PI */

/* extended precision value of PI: */
static const double DP1 = 3.14159265160560607910E0;
static const double DP2 = 1.98418714791870343106E-9;
static const double DP3 = 1.14423774522196636802E-17;

double __redupi(double x)
{
    double t;
    long i;

    t = x / M_PI;

    if (t >= 0.0) {
        t += 0.5;
    } else {
        t -= 0.5;
    }

    i = t;    /* the multiple */
    t = i;
    t = ((x - t * DP1) - t * DP2) - t * DP3;
    return t;
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
