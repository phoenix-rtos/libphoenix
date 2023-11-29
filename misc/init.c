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


void _libc_init(void)
{
	_atexit_init();
	_errno_init();
	_malloc_init();
	_env_init();
	_signals_init();
	_file_init();
	_pthread_init();
}
