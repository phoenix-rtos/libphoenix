/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * statvfs
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/statvfs.h>


int statvfs(const char *path, struct statvfs *buf)
{
	msg_t msg = { 0 };
	oid_t oid, dev;
	char *canonical;

	canonical = resolve_path(path, NULL, 1, 0);
	if (canonical == NULL) {
		/* errno set by resolve_path() */
		return -1;
	}

	if (lookup(canonical, &oid, &dev) < 0) {
		free(canonical);
		return SET_ERRNO(-ENOENT);
	}
	free(canonical);

	/* Detect mountpoint */
	if (oid.port != dev.port) {
		msg.type = mtGetAttr;
		msg.i.attr.type = atMode;
		msg.i.attr.oid = oid;

		if ((msgSend(oid.port, &msg) < 0) || (msg.o.attr.err < 0)) {
			return SET_ERRNO(-EIO);
		}

		if (S_ISDIR(msg.o.attr.val)) {
			oid = dev;
		}
	}

	memset(buf, 0, sizeof(struct statvfs));
	msg.type = mtStat;
	msg.o.data = buf;
	msg.o.size = sizeof(struct statvfs);

	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	return SET_ERRNO(msg.o.io.err);
}
