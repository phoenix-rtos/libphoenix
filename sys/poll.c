/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/poll.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/file.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>


/* hack around missing msgPoll() in kernel */


#define POLL_INTERVAL 25000


static int do_poll_iteration(struct pollfd *fds, nfds_t nfds)
{
	msg_t msg = {0};
	size_t ready, i;
	int err;

	msg.type = mtGetAttr;
	msg.i.attr.type = atPollStatus;

	for (ready = i = 0; i < nfds; ++i) {
		if (fds[i].fd < 0)
			continue;

		msg.i.attr.val = fds[i].events;

		if (fileGet(fds[i].fd, 1, &msg.i.attr.oid, NULL, NULL) < 0)
			err = -EBADF;
		else if (!(err = msgSend(msg.i.attr.oid.port, &msg)))
			err = msg.o.attr.val;

		if (err < 0)
			fds[i].revents |= POLLNVAL;
		else if (err > 0)
			fds[i].revents |= err;

		fds[i].revents &= ~(~fds[i].events & (POLLIN|POLLOUT|POLLPRI|POLLRDNORM|POLLWRNORM|POLLRDBAND|POLLWRBAND));

		if (fds[i].revents)
			++ready;
	}

	return ready;
}


int poll(struct pollfd *fds, nfds_t nfds, int timeout_ms)
{
	size_t i, n, ready;
	time_t timeout, now;

	for (i = n = 0; i < nfds; ++i) {
		fds[i].revents = 0;
		if (fds[i].fd >= 0)
			++n;
	}

	if (!n) {
		if (timeout_ms > 0)
			usleep((time_t)timeout_ms * 1000);
		return 0;
	}

	if (timeout_ms >= 0) {
		gettime(&timeout, NULL);
		timeout += timeout_ms * 1000 + !timeout_ms;
	} else
		timeout = 0;

	while (!(ready = do_poll_iteration(fds, nfds))) {
		if (timeout) {
			gettime(&now, NULL);
			now = now < timeout ? timeout - now : 1;
			if (now > POLL_INTERVAL)
				now = POLL_INTERVAL;
		} else
			now = POLL_INTERVAL;

		usleep(now);
	}

	return ready;
}
