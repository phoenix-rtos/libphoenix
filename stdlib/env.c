/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * env.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <errno.h>


int unsetenv(const char *name)
{
	return EOK;
}


int putenv(char *string)
{
	return EOK;
}


int clearenv(void)
{
	return EOK;
}
