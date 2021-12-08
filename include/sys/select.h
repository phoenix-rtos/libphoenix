/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/select.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_SELECT_H_
#define _LIBPHOENIX_SYS_SELECT_H_

#include <limits.h>
#include <signal.h>
#include <time.h>
#include <phoenix/posix/timeval.h>


/* The fd_set structure member array type */
typedef long __fd_mask;


#define FD_SETSIZE 1024                          /* Maximum number of file descriptors in the fd_set structure */
#define __NFDBITS (CHAR_BIT * sizeof(__fd_mask)) /* Number of bits per file descriptor in the fd_set structure */


typedef struct {
	__fd_mask fds_bits[(FD_SETSIZE + __NFDBITS - 1) / __NFDBITS];
} fd_set;


#define __FD_OP(n, s, op) ((s)->fds_bits[(n) / __NFDBITS] op ((__fd_mask)1 << ((n) % __NFDBITS)))

#define FD_ZERO(s) ({ \
	size_t i; \
	for (i = 0; i < sizeof((s)->fds_bits) / sizeof((s)->fds_bits[0]); ++i) \
		(s)->fds_bits[i] = 0; \
})

#define FD_CLR(n, s)   __FD_OP(n, s, &= ~)
#define FD_ISSET(n, s) __FD_OP(n, s, &)
#define FD_SET(n, s)   __FD_OP(n, s, |=)


int select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex, struct timeval *timeout);


#endif
