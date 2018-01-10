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


typedef int rlim_t;


enum { RLIM_INFINITY = -1 };


struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};


extern int getrlimit(int resource, struct rlimit *rlp);
extern int setrlimit(int resource, const struct rlimit *rlp);

#endif
