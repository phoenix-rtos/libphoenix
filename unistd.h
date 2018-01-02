/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_UNISTD_H_
#define _LIBPHOENIX_UNISTD_H_

#include ARCH


extern int vfork(void);


extern int usleep(useconds_t usecs);


extern ssize_t read(int fildes, void *buf, size_t nbyte);


extern ssize_t write(int fildes, void *buf, size_t nbyte);


#endif
