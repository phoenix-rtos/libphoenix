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

#include <errno.h>
#include <sys/events.h>
#include <phoenix/posix/poll.h>


#define POLLIN_SET  (POLLRDNORM | POLLRDBAND | POLLIN | POLLHUP | POLLERR)
#define POLLOUT_SET (POLLWRNORM | POLLWRBAND | POLLOUT | POLLHUP)
#define POLLEX_SET  (POLLPRI)
#define POLLIGN_SET (POLLERR | POLLHUP | POLLNVAL)


#define MAP_POLL_EVENT(MAP) \
	MAP(POLLOUT, evtDataOut) \
	MAP(POLLIN, evtDataIn) \
	MAP(POLLERR, evtError) \
	MAP(POLLHUP, evtGone)


extern int poll(struct pollfd *fds, nfds_t nfds, int timeout);


static inline unsigned short eventMaskFromPoll(int pollevmask)
{
#define EVENT_MASK_FROM_POLL(pollev, evt) | (!!(pollevmask & (pollev)) << (evt))

	return 0 MAP_POLL_EVENT(EVENT_MASK_FROM_POLL);

#undef EVENT_MASK_FROM_POLL
}


static inline int eventPollFromType(int evtype)
{
#define EVENT_POLL_FROM_TYPE(pollev, evt) if (evtype == (evt)) return (pollev);

	MAP_POLL_EVENT(EVENT_POLL_FROM_TYPE);
	return -EINVAL;

#undef EVENT_POLL_FROM_TYPE
}


static inline int eventPollFromMask(unsigned short evmask)
{
#define EVENT_POLL_FROM_MASK(pollev, evt) | ((evmask & (1 << (evt))) ? (pollev) : 0)

	return 0 MAP_POLL_EVENT(EVENT_POLL_FROM_MASK);

#undef EVENT_POLL_FROM_MASK
}


#endif
