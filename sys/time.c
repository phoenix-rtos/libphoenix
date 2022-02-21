/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/time.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/time.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>

#include "posix/utils.h"


extern int sys_futimes(int fd, const struct timeval times[2]);


/* path needs to be canonical */
static int utimes_internal(const char *path, const struct timeval times[2])
{
	oid_t oid, dev;
	msg_t msg = { 0 };
	struct timeval now;
	int err;

	if (lookup(path, &oid, &dev) < 0)
		return -ENOENT;

	msg.type = mtSetAttr;
	msg.i.attr.oid = oid;

	if (times == NULL)
		gettimeofday(&now, NULL);

	msg.i.attr.type = atMTime;
	msg.i.attr.val = times == NULL ? now.tv_sec : times[1].tv_sec;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;

	msg.i.attr.type = atATime;
	msg.i.attr.val = times == NULL ? now.tv_sec : times[0].tv_sec;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;

	return EOK;
}


int utimes(const char *filename, const struct timeval times[2])
{
	char *canonical;
	int err;

	if ((canonical = resolve_path(filename, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	err = utimes_internal(canonical, times);

	free(canonical);
	return SET_ERRNO(err);
}


int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t t, o;

	/* TODO: Set errno if failed */
	gettime(&t, &o);

	tp->tv_sec = (t + o) / (1000 * 1000);
	tp->tv_usec = (t + o) % (1000 * 1000);

	return 0;
}


int stime(const time_t *t)
{
	struct timeval tv;

	tv.tv_sec = *t;
	tv.tv_usec = 0;

	return settimeofday(&tv, NULL);
}


int settimeofday(const struct timeval *tv, void *tz)
{
	time_t t;

	/* TODO: Set errno if failed */
	gettime(&t, NULL);

	/* TODO: Set errno if failed */
	settime(tv->tv_usec + tv->tv_sec * 1000 * 1000 - t);

	return 0;
}


int futimes(int fd, const struct timeval tv[2])
{
	struct timeval ctv[2];
	int err;

	if (tv == NULL) {
		gettimeofday(&ctv[0], NULL);
		ctv[1] = ctv[0];
	}

	while ((err = sys_futimes(fd, tv == NULL ? ctv : tv)) == -EINTR)
		;

	return SET_ERRNO(err);
}


int lutimes(const char *filename, const struct timeval tv[2])
{
	char *canonical;
	int err;

	/* resolve_last_symlink = 0 -> utimes on a symlink */
	if ((canonical = resolve_path(filename, NULL, 0, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	err = utimes_internal(canonical, tv);

	free(canonical);
	return SET_ERRNO(err);
}


int timerisset(struct timeval *tvp)
{
	return 0;
}
