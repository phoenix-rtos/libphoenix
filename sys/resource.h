/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <sys/time.h>
#include <time.h>


typedef int rlim_t;


enum { RLIM_INFINITY = -1, RUSAGE_CHILDREN = -1 };
enum { RUSAGE_SELF };


struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
};


struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};


#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2

extern int getrlimit(int resource, struct rlimit *rlp);
extern int setrlimit(int resource, const struct rlimit *rlp);
extern int setpriority(int which, id_t who, int prio);

#endif
