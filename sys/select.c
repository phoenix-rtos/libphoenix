/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/select.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/select.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>


#define SFD_ISSET(i, s) ((s) && FD_ISSET(i, s))


WRAP_ERRNO_DEF(int, poll, (struct pollfd *fds, nfds_t nfds, int timeout_ms), (fds, nfds, timeout_ms))


static int poll_timeout(struct timeval *to)
{
	time_t timeout;

	if (to && to->tv_sec < INT_MAX / 1000) {
		timeout = ((to->tv_usec + 999) / 1000) + to->tv_sec * 1000;
		if (timeout >= 0 && timeout <= INT_MAX)
			return timeout;
	}

	return -1;
}


int select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex, struct timeval *to)
{
	struct pollfd *pfd;
	time_t timeout;
	size_t i, n;
	int rv;

	if ((nfds < 0) || (nfds > FD_SETSIZE)) {
		errno = EINVAL;
		return -1;
	}

	if ((to != NULL) && ((to->tv_usec < 0) || (to->tv_usec > 999999))) {
		errno = EINVAL;
		return -1;
	}

	for (n = i = 0; i < nfds; ++i)
		if (SFD_ISSET(i, rd) || SFD_ISSET(i, wr) || SFD_ISSET(i, ex))
			++n;

	rv = poll_timeout(to);
	timeout = rv < 0 ? 0 : (time_t)rv;

	if (n == 0) {
		rv = usleep(timeout);
		return rv < 0 ? -1 : 0;
	}

	pfd = calloc(n, sizeof(*pfd));
	if (!pfd) {
		errno = ENOMEM;
		return -1;
	}

	for (n = i = 0; i < nfds; ++i) {
		if (!(SFD_ISSET(i, rd) || SFD_ISSET(i, wr) || SFD_ISSET(i, ex)))
			continue;

		pfd[n].fd = i;
		if (SFD_ISSET(i, rd))
			pfd[n].events |= POLLIN_SET;
		if (SFD_ISSET(i, wr))
			pfd[n].events |= POLLOUT_SET;
		if (SFD_ISSET(i, ex))
			pfd[n].events |= POLLEX_SET;
		pfd[n].events &= ~POLLIGN_SET;
		++n;
	}

	rv = poll(pfd, n, timeout);

	for (i = 0; i < n; ++i) {
		if ((pfd[i].revents & POLLNVAL) != 0) {
			rv = -EBADF;
			break;
		}
	}

	if (rv < 0) {
		if (pfd)
			free(pfd);
		return SET_ERRNO(rv);
	}

	if (rd)
		FD_ZERO(rd);
	if (wr)
		FD_ZERO(wr);
	if (ex)
		FD_ZERO(ex);

	for (i = nfds = 0; i < n; ++i) {
		if (rd && (pfd[i].revents & (POLLIN_SET)))
			FD_SET(pfd[i].fd, rd);
		if (wr && (pfd[i].revents & POLLOUT_SET))
			FD_SET(pfd[i].fd, wr);
		if (ex && (pfd[i].revents & (POLLEX_SET)))
			FD_SET(pfd[i].fd, ex);
		if (SFD_ISSET(pfd[i].fd, rd) | SFD_ISSET(pfd[i].fd, wr)
				| SFD_ISSET(pfd[i].fd, ex))
			nfds++;
	}

	if (pfd)
		free(pfd);
	return nfds;
}
