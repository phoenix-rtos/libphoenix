#ifndef LIBMCS_MATH_H
#define LIBMCS_MATH_H

#ifdef __cplusplus
extern "C"{
#endif

#include "config.h"
#include "internal_config.h"

/*
 * These macros define the errno and exception behaviour of the library. This
 * library enforces the use of math_errhandling as MATH_ERREXCEPT.
 * See ISO C18 standard §7.12 wrt. math_errhandling.
 */
#define MATH_ERRNO          1
#define MATH_ERREXCEPT      2
#define math_errhandling    MATH_ERREXCEPT

typedef float  float_t;
typedef double double_t;

#define MAXFLOAT            3.40282347e+38F

#define M_E                 2.7182818284590452354
#define M_LOG2E             1.4426950408889634074
#define M_LOG10E            0.43429448190325182765
#define M_LN2               0.693147180559945309417
#define M_LN10              2.30258509299404568402
#define M_PI                3.14159265358979323846
#define M_PI_2              1.57079632679489661923
#define M_PI_4              0.78539816339744830962
#define M_1_PI              0.31830988618379067154
#define M_2_PI              0.63661977236758134308
#define M_2_SQRTPI          1.12837916709551257390
#define M_SQRT2             1.41421356237309504880
#define M_SQRT1_2           0.70710678118654752440

#define HUGE_VAL            (__infd)
#define HUGE_VALF           (__inff)
#define HUGE_VALL           ((long double) HUGE_VAL)
#define INFINITY            HUGE_VALF

/* Global constants that contain infinities. */
extern const float __inff;
extern const double __infd;

#define NAN                 (nanf(""))

#define FP_NAN              0
#define FP_INFINITE         1
#define FP_ZERO             2
#define FP_SUBNORMAL        3
#define FP_NORMAL           4

#define FP_ILOGB0           (-INT_MAX)
#define FP_ILOGBNAN         INT_MAX

/* Double trigonometric functions */
extern double               acos(double);
extern double               asin(double);
extern double               atan(double);
extern double               atan2(double, double);
extern double               cos(double);
extern double               sin(double);
extern double               tan(double);

/* Double hyperbolic functions */
extern double               acosh(double);
extern double               asinh(double);
extern double               atanh(double);
extern double               cosh(double);
extern double               sinh(double);
extern double               tanh(double);

/* Double exponential and logarithmic functions */
extern double               exp(double);
extern double               exp2(double);
extern double               expm1(double);
extern double               frexp(double, int *);
extern int                  ilogb(double);
extern double               ldexp(double, int);
extern double               log(double);
extern double               log10(double);
extern double               log1p(double);
extern double               log2(double);
extern double               logb(double);
extern double               modf(double, double *);
extern double               scalbn(double, int);
extern double               scalbln(double, long int);

/* Double power and absolute-value functions */
extern double               cbrt(double);
extern double               fabs(double);
extern double               hypot(double, double);
extern double               pow(double, double);
extern double               sqrt(double);

/* Double error and gamma functions */
extern double               erf(double);
extern double               erfc(double);
extern double               lgamma(double);
extern double               tgamma(double);

/* Double nearest integer functions */
extern double               ceil(double);
extern double               floor(double);
extern double               nearbyint(double);
extern double               rint(double);
extern long int             lrint(double);
extern long long int        llrint(double);
extern double               round(double);
extern long int             lround(double);
extern long long int        llround(double);
extern double               trunc(double);

/* Double remainder functions */
extern double               fmod(double, double);
extern double               remainder(double, double);
extern double               remquo(double, double, int *);

/* Double manipulation functions */
extern double               copysign(double, double);
extern double               nan(const char *);
extern double               nextafter(double, double);

/* Double maximum, minimum and positive difference functions */
extern double               fdim(double, double);
extern double               fmax(double, double);
extern double               fmin(double, double);

/* Double float-multiply-add function */
extern double               fma(double, double, double);

/* Double Bessel functions */
extern double               y0(double);
extern double               y1(double);
extern double               yn(int, double);
extern double               j0(double);
extern double               j1(double);
extern double               jn(int, double);


/* Float trigonometric functions */
extern float                acosf(float);
extern float                asinf(float);
extern float                atanf(float);
extern float                atan2f(float, float);
extern float                cosf(float);
extern float                sinf(float);
extern float                tanf(float);

/* Float hyperbolic functions */
extern float                acoshf(float);
extern float                asinhf(float);
extern float                atanhf(float);
extern float                coshf(float);
extern float                sinhf(float);
extern float                tanhf(float);

/* Float exponential and logarithmic functions */
extern float                expf(float);
extern float                exp2f(float);
extern float                expm1f(float);
extern float                frexpf(float, int *);
extern int                  ilogbf(float);
extern float                ldexpf(float, int);
extern float                logf(float);
extern float                log10f(float);
extern float                log1pf(float);
extern float                log2f(float);
extern float                logbf(float);
extern float                modff(float, float *);
extern float                scalbnf(float, int);
extern float                scalblnf(float, long int);

/* Float power and absolute-value functions */
extern float                cbrtf(float);
extern float                fabsf(float);
extern float                hypotf(float, float);
extern float                powf(float, float);
extern float                sqrtf(float);

/* Float error and gamma functions */
extern float                erff(float);
extern float                erfcf(float);
extern float                lgammaf(float);
extern float                tgammaf(float);

/* Float nearest integer functions */
extern float                ceilf(float);
extern float                floorf(float);
extern float                nearbyintf(float);
extern float                rintf(float);
extern long int             lrintf(float);
extern long long int        llrintf(float);
extern float                roundf(float);
extern long int             lroundf(float);
extern long long int        llroundf(float);
extern float                truncf(float);

/* Float remainder functions */
extern float                fmodf(float, float);
extern float                remainderf(float, float);
extern float                remquof(float, float, int *);

/* Float manipulation functions */
extern float                copysignf(float, float);
extern float                nanf(const char *);
extern float                nextafterf(float, float);

/* Float maximum, minimum and positive difference functions */
extern float                fdimf(float, float);
extern float                fmaxf(float, float);
extern float                fminf(float, float);

/* Float float-multiply-add function */
extern float                fmaf(float, float, float);


#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

    /* Long double trigonometric functions */
    extern long double      acosl(long double);
    extern long double      asinl(long double);
    extern long double      atanl(long double);
    extern long double      atan2l(long double, long double);
    extern long double      cosl(long double);
    extern long double      sinl(long double);
    extern long double      tanl(long double);

    /* Long double hyperbolic functions */
    extern long double      acoshl(long double);
    extern long double      asinhl(long double);
    extern long double      atanhl(long double);
    extern long double      coshl(long double);
    extern long double      sinhl(long double);
    extern long double      tanhl(long double);

    /* Long double exponential and logarithmic functions */
    extern long double      expl(long double);
    extern long double      exp2l(long double);
    extern long double      expm1l(long double);
    extern long double      frexpl(long double, int *);
    extern int              ilogbl(long double);
    extern long double      ldexpl(long double, int);
    extern long double      logl(long double);
    extern long double      log10l(long double);
    extern long double      log1pl(long double);
    extern long double      log2l(long double);
    extern long double      logbl(long double);
    extern long double      modfl(long double, long double *);
    extern long double      scalbnl(long double, int);
    extern long double      scalblnl(long double, long);

    /* Long double power and absolute-value functions */
    extern long double      cbrtl(long double);
    extern long double      fabsl(long double);
    extern long double      hypotl(long double, long double);
    extern long double      powl(long double, long double);
    extern long double      sqrtl(long double);

    /* Long double error and gamma functions */
    extern long double      erfl(long double);
    extern long double      erfcl(long double);
    extern long double      lgammal(long double);
    extern long double      tgammal(long double);

    /* Long double nearest integer functions */
    extern long double      ceill(long double);
    extern long double      floorl(long double);
    extern long double      nearbyintl(long double);
    extern long double      rintl(long double);
    extern long int         lrintl(long double);
    extern long long int    llrintl(long double);
    extern long double      roundl(long double);
    extern long             lroundl(long double);
    extern long long int    llroundl(long double);
    extern long double      truncl(long double);

    /* Long double remainder functions */
    extern long double      fmodl(long double, long double);
    extern long double      remainderl(long double, long double);
    extern long double      remquol(long double, long double, int *);

    /* Long double manipulation functions */
    extern long double      copysignl(long double, long double);
    extern long double      nanl(const char *);
    extern long double      nextafterl(long double, long double);

    /* Long double maximum, minimum and positive difference functions */
    extern long double      fdiml(long double, long double);
    extern long double      fmaxl(long double, long double);
    extern long double      fminl(long double, long double);

    /* Long double float-multiply-add function */
    extern long double      fmal(long double, long double, long double);

    /* nexttoward procedures, they're seperated from the manipulation functions
     * due to containing long double inputs for all procedures */
    extern float            nexttowardf(float, long double);
    extern double           nexttoward(double, long double);
    extern long double      nexttowardl(long double, long double);

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */

/* signgam global variable used by the lgamma procedures to return the sign of gamma */
#define signgam (__signgam)
extern int                  __signgam;

/* Internal procedures used by the classification macros */
extern int                  __fpclassifyf(float);
extern int                  __fpclassifyd(double);
extern int                  __signbitf(float);
extern int                  __signbitd(double);

/* Classification macros */
#define fpclassify(__x)     ((sizeof(__x) == sizeof(float))  ? __fpclassifyf(__x)       \
                            : __fpclassifyd(__x))
#define isfinite(__y)       (__extension__                                              \
                            ({int __cy = fpclassify(__y);                               \
                            __cy != FP_INFINITE && __cy != FP_NAN;}))

#define isinf(__x)          (fpclassify(__x) == FP_INFINITE)
#define isnan(__x)          (fpclassify(__x) == FP_NAN)
#define isnormal(__x)       (fpclassify(__x) == FP_NORMAL)

#define signbit(__x)        ((sizeof(__x) == sizeof(float))  ?  __signbitf(__x)         \
                            : __signbitd(__x))

/* Comparison macros */
#define isgreater(x,y)      (__extension__ ({                                           \
                              __typeof__(x) __x = (x); __typeof__(y) __y = (y);         \
                                int __result;                                           \
                                if (isunordered(__x, __y)) {                            \
                                    __result = 0;                                       \
                                } else {                                                \
                                    __result = (__x > __y);                             \
                                }                                                       \
                                __result;                                               \
                            }))
#define isgreaterequal(x,y) (__extension__ ({                                           \
                              __typeof__(x) __x = (x); __typeof__(y) __y = (y);         \
                                int __result;                                           \
                                if (isunordered(__x, __y)) {                            \
                                    __result = 0;                                       \
                                } else {                                                \
                                    __result = (__x >= __y);                            \
                                }                                                       \
                                __result;                                               \
                            }))
#define isless(x,y)         (__extension__ ({                                           \
                              __typeof__(x) __x = (x); __typeof__(y) __y = (y);         \
                                int __result;                                           \
                                if (isunordered(__x, __y)) {                            \
                                    __result = 0;                                       \
                                } else {                                                \
                                    __result = (__x < __y);                             \
                                }                                                       \
                                __result;                                               \
                            }))
#define islessequal(x,y)    (__extension__ ({                                           \
                              __typeof__(x) __x = (x); __typeof__(y) __y = (y);         \
                                int __result;                                           \
                                if (isunordered(__x, __y)) {                            \
                                    __result = 0;                                       \
                                } else {                                                \
                                    __result = (__x <= __y);                            \
                                }                                                       \
                                __result;                                               \
                            }))
#define islessgreater(x,y)  (__extension__ ({                                           \
                              __typeof__(x) __x = (x); __typeof__(y) __y = (y);         \
                                int __result;                                           \
                                if (isunordered(__x, __y)) {                            \
                                    __result = 0;                                       \
                                } else {                                                \
                                    __result = (__x < __y) || (__x > __y);              \
                                }                                                       \
                                __result;                                               \
                            }))
#define isunordered(a,b)    (__extension__                                              \
                            ({__typeof__(a) __a = (a); __typeof__(b) __b = (b);         \
                            fpclassify(__a) == FP_NAN || fpclassify(__b) == FP_NAN;}))

#ifdef __cplusplus
}
#endif

#endif /* !LIBMCS_MATH_H */
