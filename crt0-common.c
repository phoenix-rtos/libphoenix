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


#ifdef __FDPIC__
/* First entry in init_array and fini_array is a function pointer instead on function descriptor pointer on GCC. */
/* Use current FDPIC to call it. */
__attribute__((optimize("-O0"))) static void _init_fini_array_fdpic_call0(void *addr) {
	register void *r9 asm("r9");
	void *desc[2] = { addr, r9 };
	void (*fn)(void) = (void (*)(void))desc;
	fn();
}
#endif


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
#ifdef __FDPIC__
		/* FIXME: WHY https://patchwork.ozlabs.org/project/gcc/patch/20190515124006.25840-6-christophe.lyon@st.com/#2214345 ?? */
		/* And why our private constructors work properly. */
		/* Maybe the patch is good but support for __attribute__((constructor)) is missing. */
		/* However how did it work before on different archs? */
		if (i == 0) {
			/* This hack is so ugly that GCC on higher optimization levels has problems with it. */
			_init_fini_array_fdpic_call0(__init_array_start[0]);
		} else {
			__init_array_start[i]();
		}
#else
		__init_array_start[i]();
#endif
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
#ifdef __FDPIC__
		/* FIXME: Same as in _init_array */
		if (i == 1) {
			/* This hack is so ugly that GCC on higher optimization levels has problems with it. */
			_init_fini_array_fdpic_call0(__fini_array_start[0]);
		} else {
			__fini_array_start[i - 1]();
		}
#else
		__fini_array_start[i - 1]();
#endif
	}

	/* FIXME: change compilation settings to make access to _fini() */
	/* _fini(); */
}


extern void _libc_init(void);
extern int main(int argc, char **argv);


char **environ;
const char *argv_progname;
/* TLS cannot be used before initialized by dynamic linker in dynamically linked binaries. */
int can_use_tls;


__attribute__((noreturn)) void _startc(void (*cleanup)(void), int argc, char **argv, char **env)
{
	environ = env;
	argv_progname = *argv;

	_libc_init();

	/* At this point TLS is setup, as dynamic linker calls _startc after dealing with TLS. */
	can_use_tls = 1;

	/* cleanup function is not NULL when the dynamic linker is used */
	if (cleanup != NULL) {
		atexit(cleanup);
	}

	atexit(_fini_array);
	_init_array();

	exit(main(argc, argv));
}
