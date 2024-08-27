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

extern void __cxa_finalize(void *);
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
	__cxa_finalize(NULL);
	fflush(NULL);
	_exit(status);
	for(;;);
}
