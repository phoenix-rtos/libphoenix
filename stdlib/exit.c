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


void _exit(int status)
{
	exit(status);
}


void _Exit(int status)
{
	_exit(status);
}
