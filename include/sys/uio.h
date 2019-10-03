/*
 * Phoenix-RTOS
 *
 * libphoenix - POSIX iovec API
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

#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

#include <stdint.h>


#define UIO_MAXIOV	1024


struct iovec {
	void *iov_base;
	size_t iov_len;
};


#endif /* _SYS_UIO_H_ */
