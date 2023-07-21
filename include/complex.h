/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * mathematical functions on complex numbers
 *
 * Copyright 2023 Phoenix Systems
 * Author: Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

/* define complex numbers specifers */
#define complex    _Complex
#define _Complex_I 1.0fi
#define I          _Complex_I


#ifdef __cplusplus
extern "C" {
#endif


static inline double creal(double complex z)
{
	return (__real__(z));
}


static inline float crealf(float complex z)
{
	return (__real__(z));
}


static inline double cimag(double complex z)
{
	return (__imag__(z));
}


static inline float cimagf(float complex z)
{
	return (__imag__(z));
}


static inline double complex conj(double complex z)
{
	return __builtin_complex((__real__(z)), -(__imag__(z)));
}


static inline float complex conjf(float complex z)
{
	return __builtin_complex((__real__(z)), -(__imag__(z)));
}


double cabs(double complex);


float cabsf(float complex);


double carg(double complex);


float cargf(float complex);


double complex cexp(double complex);


float complex cexpf(float complex);


/*
 * TODO: functions not yet implemented
 */

long double cabsl(long double complex);


double complex cacos(double complex);


float complex cacosf(float complex);


double complex cacosh(double complex);


float complex cacoshf(float complex);


long double complex cacoshl(long double complex);


long double complex cacosl(long double complex);


long double cargl(long double complex);


double complex casin(double complex);


float complex casinf(float complex);


double complex casinh(double complex);


float complex casinhf(float complex);


long double complex casinhl(long double complex);


long double complex casinl(long double complex);


double complex catan(double complex);


float complex catanf(float complex);


double complex catanh(double complex);


float complex catanhf(float complex);


long double complex catanhl(long double complex);


long double complex catanl(long double complex);


double complex ccos(double complex);


float complex ccosf(float complex);


double complex ccosh(double complex);


float complex ccoshf(float complex);


long double complex ccoshl(long double complex);


long double complex ccosl(long double complex);


long double complex cexpl(long double complex);


long double cimagl(long double complex);


double complex clog(double complex);


float complex clogf(float complex);


long double complex clogl(long double complex);


long double complex conjl(long double complex);


double complex cpow(double complex, double complex);


float complex cpowf(float complex, float complex);


long double complex cpowl(long double complex, long double complex);


double complex cproj(double complex);


float complex cprojf(float complex);


long double complex cprojl(long double complex);


long double creall(long double complex);


double complex csin(double complex);


float complex csinf(float complex);


double complex csinh(double complex);


float complex csinhf(float complex);


long double complex csinhl(long double complex);


long double complex csinl(long double complex);


double complex csqrt(double complex);


float complex csqrtf(float complex);


long double complex csqrtl(long double complex);


double complex ctan(double complex);


float complex ctanf(float complex);


double complex ctanh(double complex);


float complex ctanhf(float complex);


long double complex ctanhl(long double complex);


long double complex ctanl(long double complex);


#ifdef __cplusplus
}
#endif


#endif /* end of _COMPLEX_H_ */
