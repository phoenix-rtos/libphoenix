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


int execv(const char *path, char *const argv[])
{
	return execve(path, argv, NULL);
}


int execvp(const char *file, char *const argv[])
{
	return execve(file, argv, NULL);
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


unsigned sleep(unsigned seconds)
{
	return usleep(seconds * 1000 * 1000);
}
