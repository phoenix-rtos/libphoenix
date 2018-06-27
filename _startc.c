/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>


extern void _malloc_init(void);
extern int _env_init(void);
extern int main(int argc, char **argv);

char **environ;
const char *argv_progname;



void _startc(int argc, char **argv, char **env)
{
	environ = env;
	argv_progname = *argv;

	_malloc_init();
	_env_init();

	exit(main(argc, argv));
}
