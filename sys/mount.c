/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mount
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/msg.h>

int mount(const char *path, oid_t *oid)
{
	struct stat buf;
	oid_t toid;
	msg_t msg;
	int err;

	if (lookup(path, NULL, &toid) < EOK)
		return -ENOENT;

	if ((err = stat(path, &buf)))
		return err;

	if (!S_ISDIR(buf.st_mode))
		return -ENOTDIR;

	msg.type = mtSetAttr;
	msg.i.attr.oid = toid;
	msg.i.attr.type = atDev;
	msg.i.data = oid;
	msg.i.size = sizeof(oid_t);

	if ((err = msgSend(toid.port, &msg)) < 0)
		return err;

	return 0;
}

