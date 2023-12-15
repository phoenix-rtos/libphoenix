/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint
 *
 * Copyright 2017, 2018, 2023 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Hubert Badocha
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


extern void _init(void);


__attribute__((noinline)) static void _init_array(void)
{
	size_t i, sz;

	sz = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < sz; i++) {
		__preinit_array_start[i]();
	}

	/* FIXME: change compilation settings to make access to _init() */
	/* _init(); */

	sz = __init_array_end - __init_array_start;
	for (i = 0; i < sz; i++) {
		__init_array_start[i]();
	}
}


extern void (*__fini_array_start[])(void);
extern void (*__fini_array_end[])(void);


extern void _fini(void);


static void _fini_array(void)
{
	size_t i, sz;

	sz = __fini_array_end - __fini_array_start;
	for (i = sz; i > 0; i--) {
		__fini_array_start[i - 1]();
	}

	/* FIXME: change compilation settings to make access to _fini() */
	/* _fini(); */
}


extern void _libc_init(void);
extern int main(int argc, char **argv);


char **environ;
const char *argv_progname;


__attribute__((noreturn)) void _startc(void (*cleanup)(void), int argc, char **argv, char **env)
{
	environ = env;
	argv_progname = *argv;

	_libc_init();

	/* cleanup function is not NULL when the dynamic linker is used */
	if (cleanup != NULL) {
		atexit(cleanup);
	}

	atexit(_fini_array);
	_init_array();

	exit(main(argc, argv));
}
