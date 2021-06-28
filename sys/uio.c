/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/uio.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/uio.h>


static int checkv(const struct iovec *iov, int iovcnt)
{
	unsigned int i;
	ssize_t max_len = SSIZE_MAX;

	if (iovcnt <= 0 || iovcnt > IOV_MAX)
		return -1;

	for (i = 0; i < iovcnt; ++i) {
		if (iov[i].iov_len > max_len)
			return -1;
		max_len -= iov[i].iov_len;
	}

	return 0;
}


ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
	unsigned int i;
	ssize_t n, tot_len = 0;

	if (-1 == checkv(iov, iovcnt)) {
		errno = EINVAL;
		return -1;
	}

	/* TODO: file locking */

	for (i = 0; i < iovcnt; ++i) {
		if (iov[i].iov_len > 0) {
			n = read(fd, iov[i].iov_base, iov[i].iov_len);
			if (n < 0) {
				if (tot_len > 0 && (errno == EINTR || errno == EAGAIN))
					break;
				return -1;
			}

			tot_len += n;

			if (n < iov[i].iov_len)
				break;
		}
	}

	return tot_len;
}


ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
	unsigned int i;
	ssize_t n, tot_len = 0;

	if (-1 == checkv(iov, iovcnt)) {
		errno = EINVAL;
		return -1;
	}

	/* TODO: file locking */

	for (i = 0; i < iovcnt; ++i) {
		if (iov[i].iov_len > 0) {
			n = write(fd, iov[i].iov_base, iov[i].iov_len);
			if (n < 0) {
				if (tot_len > 0 && (errno == EINTR || errno == EAGAIN))
					break;
				return -1;
			}

			tot_len += n;

			if (n < iov[i].iov_len)
				break;
		}
	}

	return tot_len;
}
