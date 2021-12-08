/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fcntl.h
 *
 * Copyright 2017, 2018, 2021 Phoenix Systems
 * Author: Aleksander Kaminski, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FCNTL_H_
#define _LIBPHOENIX_FCNTL_H_

#include <sys/types.h>
#include <phoenix/posix/fcntl.h>


extern int creat(const char *pathname, mode_t mode);


extern int fcntl(int fildes, int cmd, ...);


extern int open(const char *path, int oflag, ...);


#endif
