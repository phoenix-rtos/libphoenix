/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/resource.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/minmax.h>
#include <sys/threads.h>
#include <errno.h>

int setpriority(int which, id_t who, int prio)
{
	return SET_ERRNO(priority(min(max(prio + 4, 0), 7)));
}


int getpriority(int which, id_t who)
{
	return priority(-1) - 4;
}
