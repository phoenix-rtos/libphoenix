/*
 * Phoenix-RTOS
 *
 * libphoenix - POSIX iovec API
 *
 * sys/uio.h
 *
 * Copyright 2018, 2024 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

#include <stddef.h>
#include <sys/types.h>
#include <sys/syslimits.h>


#define UIO_MAXIOV IOV_MAX


#ifdef __cplusplus
extern "C" {
#endif


#include <phoenix/posix-uio.h>


ssize_t readv(int fd, const struct iovec *iov, int iovcnt);


ssize_t writev(int fd, const struct iovec *iov, int iovcnt);


#ifdef __cplusplus
}
#endif


#endif /* _SYS_UIO_H_ */
