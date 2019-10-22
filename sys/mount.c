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
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/msg.h>


int mount(const char *source, const char *target, const char *fstype, long mode, char *data)
{
/*	struct stat buf;
	oid_t toid, soid, doid, *doidp;
	msg_t msg = {0};
	int err;

	mount_msg_t *mnt = (mount_msg_t *)msg.i.raw;

	if (lookup(target, NULL, &toid) < EOK)
		return -ENOENT;

	if (lookup(source, NULL, &soid) < EOK)
		return -ENOENT;

	if ((err = stat(target, &buf)))
		return err;

	if (!S_ISDIR(buf.st_mode))
		return -ENOTDIR;

	msg.type = mtMount;
	mnt->id = soid.id;
	mnt->mode = mode;
	strncpy(mnt->fstype, fstype, sizeof(mnt->fstype));

	msg.i.size = data != NULL ? strlen(data) : 0;
	msg.i.data = data;

	if ((err = msgSend(soid.port, &msg)) < 0)
		return err;

	doidp = (oid_t *)msg.o.raw;
	doid = *doidp;

	memset(&msg, 0, sizeof(msg));

	msg.type = mtSetAttr;
	msg.i.attr.oid = toid;
	msg.i.attr.type = atDev;
	msg.i.data = &doid;
	msg.i.size = sizeof(oid_t);

	if ((err = msgSend(toid.port, &msg)) < 0)
		return err;
*/
	return EOK;
}
