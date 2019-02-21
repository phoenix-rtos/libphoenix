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

#ifndef _SYS_SELECT_H_
#define _SYS_SELECT_H_

#include <stdint.h>
#include <string.h>
#include <sys/time.h>


#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#define __NFDBITS (8 * (int)sizeof(uint32_t))


typedef struct fd_set_t_
{
	uint32_t fds_bits[(FD_SETSIZE + 31) / 32];
} fd_set;


#define __FD_OP(i, s, op) ((s)->fds_bits[(i) / __NFDBITS] op (1u << ((i) % __NFDBITS)))

#define FD_ZERO(s) memset((s), 0, sizeof(fd_set))
#define FD_SET(i, s) __FD_OP(i, s, |=)
#define FD_CLR(i, s) __FD_OP(i, s, &= ~)
#define FD_ISSET(i, s) !!__FD_OP(i, s, &)


int select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex, struct timeval *timeout);


#endif
