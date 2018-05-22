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

#include ARCH

typedef int rlim_t;


enum { RLIM_INFINITY = -1 };


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
