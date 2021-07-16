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
	int ret = priority(min(max(prio + 4, 0), 7));
	return SET_ERRNO(ret < 0 ? ret : 0);
}


int getpriority(int which, id_t who)
{
	int ret = priority(-1);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}
	else
		return (ret - 4);
}
