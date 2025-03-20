/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 *
 * _setjmp, _longjmp, setjmp, sigsetjmp
 *
 * Copyright 2025 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <setjmp.h>
#include <stdlib.h>


int _setjmp(jmp_buf var)
{
	abort();
}


__attribute__((__noreturn__)) void _longjmp(jmp_buf var, int m)
{
	abort();
}


int setjmp(jmp_buf var)
{
	abort();
}


int sigsetjmp(sigjmp_buf env, int savesigs)
{
	abort();
}
