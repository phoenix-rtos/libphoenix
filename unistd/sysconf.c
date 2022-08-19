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
#include <sys/syslimits.h>


long sysconf(int name)
{
	switch (name) {
	case _SC_OPEN_MAX:
		/* value got from MAX_FD_COUNT (kernel) */
		/* TODO: come up with a solution to access a macro defined in kernel posix module */
		return 512;
	case _SC_IOV_MAX:
		return IOV_MAX;
	default:
		errno = EINVAL;
		return -1;
	}
}
