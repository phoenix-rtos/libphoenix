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

#include <sys/minmax.h>
#include <sys/select.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#include "../common/util.h"

/* POSIX requires the maximum timeout in select to be at least 31 days */
#define POSIX_MAX_TIMEOUT_MS (31LL * 24LL * 60LL * 60LL * 1000LL)

#define SFD_ISSET(i, s) ((s) && FD_ISSET(i, s))


/* clang-format off */
WRAP_ERRNO_DEF(int, poll, (struct pollfd *fds, nfds_t nfds, int timeout_ms), (fds, nfds, timeout_ms))
/* clang-format on */


extern int nsleep(time_t *sec, long *nsec, int clockid, int flags);


int select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex, struct timeval *to)
{
	struct pollfd *pfd;
	time_t timeoutMs, sec;
	size_t i, n;
	int rv;
	long nsec;
	long long requestedTimeoutMs;

	if ((nfds < 0) || (nfds > FD_SETSIZE)) {
		return SET_ERRNO(-EINVAL);
	}

	if (to == NULL) {
		requestedTimeoutMs = -1;
	}
	else {
		if (!__timevalValid(to) || to->tv_sec < 0) {
			return SET_ERRNO(-EINVAL);
		}

		if (to->tv_sec > POSIX_MAX_TIMEOUT_MS / 1000) {
			requestedTimeoutMs = POSIX_MAX_TIMEOUT_MS;
		}
		else {
			requestedTimeoutMs = min((((long long)to->tv_usec + 999) / 1000) + ((long long)to->tv_sec * 1000), POSIX_MAX_TIMEOUT_MS);
		}
	}

	for (n = i = 0; i < nfds; ++i) {
		if (SFD_ISSET(i, rd) || SFD_ISSET(i, wr) || SFD_ISSET(i, ex)) {
			++n;
		}
	}

	if (n == 0) {
		if (to == NULL) {
			rv = pause();
		}
		else {
			sec = min(to->tv_sec, POSIX_MAX_TIMEOUT_MS / 1000);
			nsec = to->tv_usec * 1000;
			if (sec != 0 || nsec != 0) {
				rv = SET_ERRNO(nsleep(&sec, &nsec, CLOCK_MONOTONIC, 0));
			}
			else {
				rv = 0;
				sched_yield();
			}
		}
		return rv < 0 ? -1 : 0;
	}

	pfd = calloc(n, sizeof(*pfd));
	if (pfd == NULL) {
		return SET_ERRNO(-ENOMEM);
	}

	for (n = i = 0; i < nfds; ++i) {
		if (!(SFD_ISSET(i, rd) || SFD_ISSET(i, wr) || SFD_ISSET(i, ex))) {
			continue;
		}

		pfd[n].fd = i;
		if (SFD_ISSET(i, rd)) {
			pfd[n].events |= POLLIN_SET;
		}
		if (SFD_ISSET(i, wr)) {
			pfd[n].events |= POLLOUT_SET;
		}
		if (SFD_ISSET(i, ex)) {
			pfd[n].events |= POLLEX_SET;
		}
		pfd[n].events &= ~POLLIGN_SET;
		++n;
	}

	do {
		timeoutMs = (requestedTimeoutMs < 0) ? -1 : min(requestedTimeoutMs, INT_MAX);
		rv = poll(pfd, n, (int)timeoutMs);
		requestedTimeoutMs -= timeoutMs;
	} while ((rv == 0) && (requestedTimeoutMs > 0));

	for (i = 0; i < n; ++i) {
		if ((pfd[i].revents & POLLNVAL) != 0) {
			rv = SET_ERRNO(-EBADF);
			break;
		}
	}

	if (rv < 0) {
		free(pfd);
		/* errno set by poll */
		return rv;
	}

	if (rd != NULL) {
		FD_ZERO(rd);
	}
	if (wr != NULL) {
		FD_ZERO(wr);
	}
	if (ex != NULL) {
		FD_ZERO(ex);
	}

	for (i = nfds = 0; i < n; ++i) {
		if (rd != NULL && (pfd[i].revents & POLLIN_SET) != 0) {
			FD_SET(pfd[i].fd, rd);
		}
		if (wr != NULL && (pfd[i].revents & POLLOUT_SET) != 0) {
			FD_SET(pfd[i].fd, wr);
		}
		if (ex != NULL && (pfd[i].revents & POLLEX_SET) != 0) {
			FD_SET(pfd[i].fd, ex);
		}
		if (SFD_ISSET(pfd[i].fd, rd) | SFD_ISSET(pfd[i].fd, wr) | SFD_ISSET(pfd[i].fd, ex)) {
			nfds++;
		}
	}

	free(pfd);
	return nfds;
}
