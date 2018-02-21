/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/wait.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

enum {
	WNOHANG = 1 << 0,
	WUNTRACED = 1 << 1,
};


#define WIFEXITED(stat_val) 0
#define WEXITSTATUS(stat_val) 0
#define WIFSIGNALED(stat_val) 0
#define WTERMSIG(stat_val) 0
#define WIFSTOPPED(stat_val) 0
#define WSTOPSIG(stat_val) 0
#define WIFCONTINUED(stat_val) 0
#define WCOREDUMP(stat_val) 0


extern pid_t waitpid(pid_t pid, int *status, int options);


static pid_t wait(int *status)
{
	return waitpid(-1, status, 0);
}




#endif
