/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * abort.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <signal.h>
#include <stdlib.h>


void abort(void)
{
	raise(SIGABRT);
	while(1)
		exit(EXIT_FAILURE);
}
