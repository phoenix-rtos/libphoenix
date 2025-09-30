/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#include "fenv.h"

int feclearexcept(int excepts)
{
    return -1;
}

int feraiseexcept(int excepts)
{
    return -1;
}

int fegetexceptflag(fexcept_t *flagp, int excepts)
{
    return -1;
}

int fesetexceptflag(const fexcept_t *flagp, int excepts)
{
    return -1;
}

int fegetround(void)
{
    return -1;
}

int fesetround(int rdir)
{
    return -1;
}

int fegetenv(fenv_t *envp)
{
    return -1;
}

int fesetenv(const fenv_t *envp)
{
    return -1;
}

int feholdexcept(fenv_t *envp)
{
    return -1;
}

int feupdateenv(const fenv_t *envp)
{
    return -1;
}

int fetestexcept(int excepts)
{
    return -1;
}
