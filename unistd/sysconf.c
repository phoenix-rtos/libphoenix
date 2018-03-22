/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for user operations)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <unistd.h>


long sysconf(int name)
{
	switch (name) {
	case _SC_OPEN_MAX:
		return (1u << 31) - 1;
	default:
		errno = EINVAL;
		return -1;
	}
}
