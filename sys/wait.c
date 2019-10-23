/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/wait
 *
 * Copyright 2018 Phoenix Systems
 * Author: Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <sys/wait.h>

extern int ProcWait(pid_t pid, int *stat, int options);

pid_t waitpid(pid_t pid, int *status, int options)
{
	return SET_ERRNO(ProcWait(pid, status, options));
}

