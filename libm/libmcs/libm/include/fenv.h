/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#ifndef LIBMCS_FENV_H
#define LIBMCS_FENV_H

#error fenv.h/fenv.c shall not be used as is. They have no functionality \
       other than returning an error value and providing prototypes.     \
       If you, the user, want to use fenv you will have to implement the \
       features yourself (or copy them from somewhere). We can not       \
       provide these functionalities for you as their implementation is  \
       highly platform dependant.

#ifdef __cplusplus
extern "C"{
#endif

/* Floating-point Exceptions */
extern int feclearexcept(int);
extern int feraiseexcept(int);
extern int fegetexceptflag(fexcept_t *, int);
extern int fesetexceptflag(const fexcept_t *, int);

/* Rounding Direction */
extern int fegetround(void);
extern int fesetround(int);

/* Entire Environment */
extern int fegetenv(fenv_t *);
extern int fesetenv(const fenv_t *);
extern int feholdexcept(fenv_t *);
extern int feupdateenv(const fenv_t *);

/* Other */
extern int fetestexcept(int);

#ifdef __cplusplus
}
#endif

#endif /* !LIBMCS_FENV_H */
