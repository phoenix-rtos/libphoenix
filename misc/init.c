/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * init
 *
 * Copyright 2021, 2023 Phoenix Systems
 * Author: Hubert Buczynski, Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


extern void _malloc_init(void);
extern int _env_init(void);
extern void _signals_init(void);
extern void _file_init(void);
extern void _errno_init(void);
extern void _atexit_init(void);
extern void _init_array(void);
extern void _pthread_init(void);


static int _libc_initialized;


/* _libc_init is called twice once explicitly in crt0 and once during constructor handling.
 * If libc is linked statically first it is called explicitly and then during constructor handling.
 * If libc is linked dynamically first it is called by the constructor handling mechanism in dynamic linker,
 * to allow constructors from other objects to call libc functions.
 */
__attribute__((constructor)) void _libc_init(void)
{
	if (_libc_initialized != 0) {
		return;
	}
	_libc_initialized = 1;

	_atexit_init();
	_errno_init();
	_malloc_init();
	_env_init();
	_signals_init();
	_file_init();
	_pthread_init();
}
