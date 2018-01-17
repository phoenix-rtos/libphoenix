/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (ia32)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH
#include <sys/file.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>


extern int main(int argc, char **argv);


char **environ;
FILE stdin_file, stdout_file;

void _start(int argc, char **argv, char **env)
{
	unsigned int h;
	oid_t oid = { 0, 0 };

	environ = env;
	optind = 1;

	stdin = &stdin_file;
	stdout = &stdout_file;

	fileAdd(&h, &oid);
	fileAdd(&h, &oid);
	fileAdd(&h, &oid);

	_malloc_init();

	exit(main(argc, argv));
}
