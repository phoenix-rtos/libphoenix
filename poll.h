/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * poll.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHENIX_POLL_H_
#define _LIBPHENIX_POLL_H_


#define POLLIN 1


typedef unsigned int nfds_t;


struct pollfd {
	int   fd;
	short events;
	short revents;
};


extern int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif
