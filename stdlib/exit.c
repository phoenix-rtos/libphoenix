/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Terminating a process
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <stdlib.h>


extern void _atexit_call(void);
extern void _fini_array(void);
extern void sys_exit(int) __attribute__((noreturn));


void _exit(int status)
{
	sys_exit(status);
}


void _Exit(int status)
{
	sys_exit(status);
}


void exit(int status)
{
	fflush(NULL);
	_atexit_call();
	_fini_array();
	_exit(status);
	for(;;);
}
