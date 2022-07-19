/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/select.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SELECT_H_
#define _SYS_SELECT_H_

#include <time.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};


#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#define __NFDBITS (8 * (int)sizeof(uint32_t))


typedef struct fd_set_t_
{
	uint32_t fds_bits[(FD_SETSIZE + 31) / 32];
} fd_set;


#define __FD_OP(i, s, op) ((s)->fds_bits[(i) / __NFDBITS] op (1u << ((i) % __NFDBITS)))

#define FD_ZERO(s) \
do { \
	uint32_t i; \
	for (i = 0; i < sizeof((s)->fds_bits) / sizeof((s)->fds_bits[0]); ++i) \
		(s)->fds_bits[i] = 0; \
} while (0);
#define FD_SET(i, s) __FD_OP(i, s, |=)
#define FD_CLR(i, s) __FD_OP(i, s, &= ~)
#define FD_ISSET(i, s) !!__FD_OP(i, s, &)


int select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex, struct timeval *timeout);


#ifdef __cplusplus
}
#endif


#endif
