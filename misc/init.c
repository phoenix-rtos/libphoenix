/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * init array
 *
 * Copyright 2021 Phoenix Systems
 * Author: Hubert Buczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stdlib.h>

extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);

extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);


extern void _init (void);

void _init_array(void)
{
	size_t i, sz;

	sz = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < sz; i++)
		__preinit_array_start[i]();

	/* FIXME: change compilation settings to make access to _init() */
	/* _init(); */

	sz = __init_array_end - __init_array_start;
	for (i = 0; i < sz; i++)
		__init_array_start[i]();
}
