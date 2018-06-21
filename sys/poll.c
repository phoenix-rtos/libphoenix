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
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>


// FIXME: no poll() in kernel -> single socket poll only for now
static int do_poll(struct pollfd *fds, nfds_t nfds, int timeout_)
{
	struct pollfd *fd = fds;
	unsigned mode;
	time_t timeout;
	size_t i, n;

	for (i = n = 0; i < nfds; ++i) {
		if (fds[i].fd >= 0) {
			++n;
			fd = fds + i;
		} else
			fds[i].revents = 0;
	}

	if (n > 1)
		return -ENOSYS;

	timeout = timeout_ >= 0 ? timeout_ * 1000 : timeout_;

	if (!n) {
		if (timeout > 0)
			usleep(timeout);
		return 0;
	}

	if (fileGet(fd->fd, 4, NULL, NULL, &mode) < 0) {
		fd->revents = POLLNVAL;
		return 1;
	}

	if (!S_ISSOCK(mode))
		return -ENOSYS;

	fd->revents = __sock_poll(fd->fd, fd->events, timeout);

	return fd->revents ? 1 : 0;
}


int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	int ret = do_poll(fds, nfds, timeout);

	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}
