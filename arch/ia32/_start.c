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


extern int main(int argc, char **argv);


char **environ;


void _start(void)
{
	char *argv[6] = { NULL };
	char **p = argv;
	unsigned int h;
	oid_t oid = { 0, 0 };

	environ = NULL;

	*(p++) = "sh";
	*(p++) = "-i";

	fileAdd(&h, &oid);
	fileAdd(&h, &oid);
	fileAdd(&h, &oid);

	_malloc_init();
	exit(main(p - argv, argv));
}
