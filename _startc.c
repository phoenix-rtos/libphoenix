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

#include ARCH
#include <sys/file.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>


extern int main(int argc, char **argv);

typedef void (*init_func_t)(void);

extern const init_func_t __init_array_start[];
extern const init_func_t __init_array_end[];


char **environ;
FILE stdin_file, stdout_file;

void _startc(int argc, char **argv, char **env)
{
	const init_func_t *fp;
	init_func_t f;

	environ = env;
	optind = 1;
	stdin = &stdin_file;
	stdout = &stdout_file;

	for (fp = __init_array_start; fp != __init_array_end; ++fp) {
		f = *fp;
		if (!f || !~(unsigned)f)
			continue;
		f();
	}

	exit(main(argc, argv));
}
