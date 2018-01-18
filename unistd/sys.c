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
#include <unistd.h>
#include <errno.h>


long sysconf(int name)
{
	return -ENOSYS;
}


int execve(const char *path, char *const argv[], char *const envp[])
{
	return execle(path, argv[0]);
}


pid_t fork(void)
{
	return -ENOSYS;
}


int execv(const char *path, char *const argv[])
{
	return execle(path, argv[0]);
}


int execvp(const char *file, char *const argv[])
{
	return execle(file, argv[0]);
}


/* TODO :move */
unsigned int major(int dev)
{
	return 0;
}


unsigned int minor(int dev)
{
	return 0;
}
