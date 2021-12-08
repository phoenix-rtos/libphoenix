/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/uio.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_UIO_H_
#define _LIBPHOENIX_SYS_UIO_H_

#include <limits.h>
#include <sys/types.h>
#include <phoenix/posix/iovec.h>


ssize_t readv(int fd, const struct iovec *iov, int iovcnt);


ssize_t writev(int fd, const struct iovec *iov, int iovcnt);


#endif
