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

#include <stdlib.h>
#include <stdio.h>

extern void _atexit_call(void);
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
	_exit(status);
	for(;;);
}
