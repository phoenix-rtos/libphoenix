/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fini array
 *
 * Copyright 2021 Phoenix Systems
 * Author: Hubert Buczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>


extern void (*__fini_array_start[])(void);
extern void (*__fini_array_end[])(void);

extern void _fini (void);


void _fini_array(void)
{
	size_t i, sz;

	sz = __fini_array_end - __fini_array_start;
	for (i = sz; i > 0; i--)
		__fini_array_start[i-1]();

	/* FIXME: change compilation settings to make access to _fini() */
	/* _fini(); */
}
