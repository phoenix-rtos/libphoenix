/* SPDX-License-Identifier: NetBSD */
/* Written by Matthias Drochner. */

#ifndef LIBMCS_COMPLEX_H
#define LIBMCS_COMPLEX_H

#ifdef __STDC_NO_COMPLEX__
    #error Your toolchain has defined __STDC_NO_COMPLEX__ which indicates that \
           it does not support complex types as such you should not include \
           complex.h in your software. (Hint: When running configure for the \
           LibmCS choose not to use complex procedures.)
#endif

#ifdef __cplusplus
extern "C"{
#endif

#include "config.h"
#include "internal_config.h"

#ifndef __LIBMCS_WANT_COMPLEX
    #error During the configure step you have chosen not to compile complex \
           procedures as such you should not include complex.h in your software.
#endif

#define complex _Complex
#define _Complex_I __extension__ 1.0fi
#define I _Complex_I

/* 7.3.5 Trigonometric functions */
/* 7.3.5.1 The cacos functions */
double complex cacos(double complex);
float complex cacosf(float complex);

/* 7.3.5.2 The casin functions */
double complex casin(double complex);
float complex casinf(float complex);

/* 7.3.5.1 The catan functions */
double complex catan(double complex);
float complex catanf(float complex);

/* 7.3.5.1 The ccos functions */
double complex ccos(double complex);
float complex ccosf(float complex);

/* 7.3.5.1 The csin functions */
double complex csin(double complex);
float complex csinf(float complex);

/* 7.3.5.1 The ctan functions */
double complex ctan(double complex);
float complex ctanf(float complex);

/* 7.3.6 Hyperbolic functions */
/* 7.3.6.1 The cacosh functions */
double complex cacosh(double complex);
float complex cacoshf(float complex);

/* 7.3.6.2 The casinh functions */
double complex casinh(double complex);
float complex casinhf(float complex);

/* 7.3.6.3 The catanh functions */
double complex catanh(double complex);
float complex catanhf(float complex);

/* 7.3.6.4 The ccosh functions */
double complex ccosh(double complex);
float complex ccoshf(float complex);

/* 7.3.6.5 The csinh functions */
double complex csinh(double complex);
float complex csinhf(float complex);

/* 7.3.6.6 The ctanh functions */
double complex ctanh(double complex);
float complex ctanhf(float complex);

/* 7.3.7 Exponential and logarithmic functions */
/* 7.3.7.1 The cexp functions */
double complex cexp(double complex);
float complex cexpf(float complex);

/* 7.3.7.2 The clog functions */
double complex clog(double complex);
float complex clogf(float complex);

/* 7.3.8 Power and absolute-value functions */
/* 7.3.8.1 The cabs functions */
double cabs(double complex) ;
float cabsf(float complex) ;

/* 7.3.8.2 The cpow functions */
double complex cpow(double complex, double complex);
float complex cpowf(float complex, float complex);

/* 7.3.8.3 The csqrt functions */
double complex csqrt(double complex);
float complex csqrtf(float complex);

/* 7.3.9 Manipulation functions */
/* 7.3.9.1 The carg functions */
double carg(double complex);
float cargf(float complex);

/* 7.3.9.2 The cimag functions */
double cimag(double complex);
float cimagf(float complex);

/* 7.3.9.3 The conj functions */
double complex conj(double complex);
float complex conjf(float complex);

/* 7.3.9.4 The cproj functions */
double complex cproj(double complex);
float complex cprojf(float complex);

/* 7.3.9.5 The creal functions */
double creal(double complex);
float crealf(float complex);

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

    long double complex cacosl(long double complex);
    long double complex casinl(long double complex);
    long double complex catanl(long double complex);
    long double complex ccosl(long double complex);
    long double complex csinl(long double complex);
    long double complex ctanl(long double complex);

    long double complex cacoshl(long double complex);
    long double complex casinhl(long double complex);
    long double complex catanhl(long double complex);
    long double complex ccoshl(long double complex);
    long double complex csinhl(long double complex);
    long double complex ctanhl(long double complex);

    long double complex cexpl(long double complex);
    long double complex clogl(long double complex);

    long double cabsl(long double complex) ;
    long double complex cpowl(long double complex, long double complex);
    long double complex csqrtl(long double complex);

    long double cargl(long double complex);
    long double cimagl(long double complex);
    long double complex conjl(long double complex);
    long double complex cprojl(long double complex);
    long double creall(long double complex);

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */

/* The C11 CMPLX macros are compiler dependant and only available starting at
 * GCC 4.7+ or with clang! If need be define them yourself. */
#if defined(__clang__) || (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
    #define CMPLX(x, y)     __builtin_complex ((double) (x), (double) (y))
    #define CMPLXF(x, y)    __builtin_complex ((float) (x), (float) (y))
    #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS
        #define CMPLXL(x, y)    __builtin_complex ((long double) (x), (long double) (y))
    #endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#else
    /* Due to the used compiler being too old the library cannot provide fully
     * C11 standard compliant macros CMPLX, CMPLXF, CMPLXL. The library
     * provides functionally equivalent inline functions with the same symbol
     * name with the only limitation in that they cannot be used for static
     * initialisation. */

    static inline float complex CMPLXF(float x, float y)
    {
        union {
            float a[2];
            float complex f;
        } z = {{ x, y }};

        return (z.f);
    }

    static inline double complex CMPLX(double x, double y)
    {
        union {
            double a[2];
            double complex f;
        } z = {{ x, y }};

        return (z.f);
    }

    #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS
        static inline long double complex CMPLXL(long double x, long double y)
        {
            union {
                long double a[2];
                long double complex f;
            } z = {{ x, y }};

            return (z.f);
        }
    #endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif

#ifdef __cplusplus
}
#endif

#endif /* !LIBMCS_COMPLEX_H */
