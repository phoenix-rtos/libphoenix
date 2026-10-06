/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2026 by GTD GmbH. */

/* clang-format off */
#ifndef LIBMCS_FENV_H
#define LIBMCS_FENV_H

#ifdef __cplusplus
extern "C"{
#endif

typedef uintptr_t fexcept_t;

typedef struct _fenv_t {
	int dummy;
} fenv_t;

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
/* clang-format on */
