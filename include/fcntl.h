/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fcntl.h
 *
 * Copyright 2017, 2018, 2024 Phoenix Systems
 * Author: Aleksander Kaminski, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FCNTL_H_
#define _LIBPHOENIX_FCNTL_H_


#include <sys/types.h>

#include <phoenix/posix-fcntl.h>


#ifdef __cplusplus
extern "C" {
#endif


int fcntl(int fildes, int cmd, ...);


int open(const char *path, int oflag, ...);


int creat(const char *pathname, mode_t mode);


enum {
	LOCK_SH = 1,
	LOCK_EX = 2,
	LOCK_NB = 4,
	LOCK_UN = 8,
};


enum { F_RDLCK, F_UNLCK, F_WRLCK };


struct flock {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};


#ifdef __cplusplus
}
#endif


#endif
