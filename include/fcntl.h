/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fcntl.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FCNTL_H_
#define _LIBPHOENIX_FCNTL_H_

#include <phoenix/fcntl.h>


int fcntl(int fildes, int cmd, ...);

int open(const char *path, int oflag, ...);

int openat(int dirfd, const char *path, int oflag, ...);

int creat(const char* path, int mode);

#endif
