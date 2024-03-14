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
	char *canonical = resolve_path(path, NULL, 1, 0);
	if (canonical == NULL) {
		/* errno set by resolve_path() */
		return -1;
	}

	oid_t oid, dev;
	if (lookup(canonical, &oid, &dev) < 0) {
		free(canonical);
		return SET_ERRNO(-ENOENT);
	}
	free(canonical);

	/* Detect mountpoint */
	if (oid.port != dev.port) {
		msg_t msg = {
			.type = mtGetAttr,
			.oid = oid,
			.i.attr.type = atMode
		};

		if ((msgSend(oid.port, &msg) < 0) || (msg.o.err < 0)) {
			return SET_ERRNO(-EIO);
		}

		if (S_ISDIR(msg.o.attr.val)) {
			oid = dev;
		}
	}

	memset(buf, 0, sizeof(struct statvfs));

	msg_t msg = {
		.type = mtStat,
		.o.data = buf,
		.o.size = sizeof(struct statvfs)
	};

	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	return SET_ERRNO(msg.o.err);
}
