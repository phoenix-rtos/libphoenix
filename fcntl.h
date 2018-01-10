/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fcntl.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FCNTL_H_
#define _LIBPHOENIX_FCNTL_H_

#define O_RDONLY 0x0001
#define O_WRONLY 0x0002
#define O_RDWR 0x0004
#define O_APPEND 0x0008
#define O_CREAT 0x0100
#define O_TRUNC 0x0200
#define O_EXCL 0x0400
#define O_NONBLOCK 0x1000
#define O_NOCTTY 0x2000

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


int fcntl(int fildes, int cmd, ...);


/* cmd's for fcntl */
enum { F_DUPFD, F_DUPFD_CLOEXEC, F_GETFD, F_SETFD, F_GETFL, F_SETFL,
       F_GETOWN, F_SETOWN, F_GETLK, F_SETLK, F_SETLKW };


enum { FD_CLOEXEC = 1 };

#endif
