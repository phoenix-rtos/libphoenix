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
extern int _startc(int argc, char **argv, char **env);


void _start(int argc, char **argv, char **env)
{
	_startc(argc, argv, env);
}
