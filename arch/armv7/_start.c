/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (armv7)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

__attribute__((noreturn))
extern int _startc(void);


void _start(void)
{
	_startc();
}
