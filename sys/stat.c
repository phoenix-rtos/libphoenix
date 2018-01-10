/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/stat.h>
#include <errno.h>

int fstat(int fildes, struct stat *buf)
{
	return -ENOSYS;
}


int stat(const char *restrict path, struct stat *restrict buf)
{
	return -ENOSYS;
}


mode_t umask(mode_t cmask)
{
	return -ENOSYS;
}


int lstat(const char *restrict path, struct stat *restrict buf)
{
	return -ENOSYS;
}


int creat(const char *pathname, int mode)
{
	return -ENOSYS;
}
