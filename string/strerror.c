/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/strerror (errno names)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stddef.h>


static const char *const errnames[] = {
	#include "errstr.inc"
};

const char *strerror(int errnum)
{
	if (errnum < 0)
		errnum = -errnum;
	if ((unsigned)errnum >= sizeof(errnames) / sizeof(*errnames))
		return NULL;
	return errnames[errnum];
}
