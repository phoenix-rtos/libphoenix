/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * glob.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <glob.h>
#include <errno.h>


int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno), glob_t *pglob)
{
	return -ENOSYS;
}


void globfree(glob_t *pglob)
{
}
