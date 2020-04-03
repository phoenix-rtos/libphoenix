/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * atexit.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <string.h>

void (*atexit_funcs[ATEXIT_MAX])(void);


void _atexit_call(void)
{
	int i;

	for (i = ATEXIT_MAX - 1; i >= 0; i--) {
		if (atexit_funcs[i] != NULL)
			atexit_funcs[i]();
	}
}


#warning FIXME: atexit() uses GCC built-in function defined in Intel Pentium ABI! Works only with -mcpu=i586

int atexit(void (*func)(void))
{
	int i;

	for (i = 0; i < ATEXIT_MAX; i++) {
		if (__sync_bool_compare_and_swap(&atexit_funcs[i], NULL, func))
			return 0;
	}

	return -1;
}
