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

#include <sys/events.h>
#include <phoenix/poll.h>
#include <errno.h>

typedef unsigned int nfds_t;


extern int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif
