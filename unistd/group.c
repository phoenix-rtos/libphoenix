/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for group operations)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <sys/types.h>


gid_t getgid(void)
{
	return 0;
}


gid_t getegid(void)
{
	return 0;
}


int getgroups(int size, gid_t list[])
{
	return 0;
}


int setgroups(size_t size, const gid_t *list)
{
	return 0;
}

int issetugid(void)
{
	return 0;
}
