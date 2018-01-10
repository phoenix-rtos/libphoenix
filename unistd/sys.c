/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/types.h>
#include <errno.h>


long sysconf(int name)
{
	return -ENOSYS;
}


int execve(const char *path, char *const argv[], char *const envp[])
{
	return -ENOSYS;
}


pid_t fork(void)
{
	return -ENOSYS;
}


int execv(const char *path, char *const argv[])
{
	return -ENOSYS;
}


int execvp(const char *file, char *const argv[])
{
	return -ENOSYS;
}
