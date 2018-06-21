/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/poll.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_POLL_H_
#define _SYS_POLL_H_

#include <stdint.h>


#define POLLIN		0x0001
#define POLLOUT		0x0002
#define POLLPRI		0x0004
#define POLLERR		0x0008
#define POLLHUP		0x0010
#define POLLNVAL	0x0020

#define POLLRDNORM	POLLIN
#define POLLRDBAND	POLLIN
#define POLLWRNORM	POLLOUT
#define POLLWRBAND	POLLOUT


typedef size_t nfds_t;

struct pollfd {
	int fd;
	short int events;
	short int revents;
};


int poll(struct pollfd *fds, nfds_t nfds, int timeout);


#endif
