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
	struct stat buf;
	oid_t toid, soid, doid, *doidp;
	msg_t msg = {0};
	char *source_abspath, *target_abspath;
	int err;

	mount_msg_t *mnt = (mount_msg_t *)msg.i.raw;

	if ((target_abspath = resolve_path(target, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	if ((source_abspath = resolve_path(source, NULL, 1, 0)) == NULL) {
		free(target_abspath);
		return -1; /* errno set by resolve_path */
	}

	if (lookup(target_abspath, NULL, &toid) < 0) {
		free(target_abspath);
		free(source_abspath);
		return SET_ERRNO(-ENOENT);
	}

	if (lookup(source_abspath, NULL, &soid) < 0) {
		free(target_abspath);
		free(source_abspath);
		return SET_ERRNO(-ENOENT);
	}

	err = stat(target_abspath, &buf);

	free(target_abspath);
	free(source_abspath);

	if (err < 0)
		return err;

	if (!S_ISDIR(buf.st_mode))
		return SET_ERRNO(-ENOTDIR);

	msg.type = mtMount;
	mnt->id = soid.id;
	mnt->mode = mode;
	strncpy(mnt->fstype, fstype, sizeof(mnt->fstype));
	mnt->fstype[sizeof(mnt->fstype) - 1] = '\0';

	msg.i.size = data != NULL ? strlen(data) : 0;
	msg.i.data = data;

	if (msgSend(soid.port, &msg) < 0)
		return SET_ERRNO(-EIO);

	doidp = (oid_t *)msg.o.raw;
	doid = *doidp;

	memset(&msg, 0, sizeof(msg));

	msg.type = mtSetAttr;
	msg.i.attr.oid = toid;
	msg.i.attr.type = atDev;
	msg.i.data = &doid;
	msg.i.size = sizeof(oid_t);

	if (msgSend(toid.port, &msg) < 0)
		return SET_ERRNO(-EIO); /* FIXME: rollback partial mount */

	return SET_ERRNO(msg.o.attr.err);
}


int umount(const char *target)
{
	msg_t msg = { 0 };
	mount_mnt_t *mnt = (mount_mnt_t *)msg.o.raw;
	int rootfs = 0;
	oid_t oid, dev;
	char *abspath;

	abspath = resolve_path(target, NULL, 1, 0);
	if (abspath == NULL) {
		/* errno set by resolve_path() */
		return -1;
	}

	if (lookup(abspath, &oid, &dev) < 0) {
		free(abspath);
		return SET_ERRNO(-ENOENT);
	}

	/* Check for attached device */
	if (oid.port == dev.port) {
		/* Check for rootfs */
		if ((abspath[0] == '/') && (abspath[1] == '\0')) {
			rootfs = 1;
		}
		/* Invalid target, neither device nor mountpoint */
		else {
			free(abspath);
			return SET_ERRNO(-EINVAL);
		}
	}
	free(abspath);

	/* Check file type */
	msg.type = mtGetAttr;
	msg.i.attr.type = atMode;
	msg.i.attr.oid = oid;

	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	if (msg.o.attr.err < 0) {
		return SET_ERRNO(msg.o.attr.err);
	}

	/* Got mountpoint */
	if (S_ISDIR(msg.o.attr.val)) {
		/* TODO: implement umount() by mountpoint, set ENOTSUP errno for now */
		return SET_ERRNO(-ENOTSUP);
	}
	/* Got mounted device */
	/* TODO: check device type (should be S_IFBLK?) */
	else {
		/* Get mountpoint */
		msg.type = mtMnt;
		msg.i.data = &dev;
		msg.i.size = sizeof(dev);

		if (msgSend(dev.port, &msg) < 0) {
			return SET_ERRNO(-EIO);
		}

		if (mnt->err < 0) {
			/* Check for rootfs */
			if (mnt->err == -ENOENT) {
				rootfs = 1;
			}
			/* No mountpoint */
			else {
				return SET_ERRNO(mnt->err);
			}
		}
		else {
			oid = mnt->mnt;
		}
	}

	/* Unmount filesystem */
	msg.type = mtUmount;
	msg.i.data = &dev;
	msg.i.size = sizeof(dev);

	if (msgSend(dev.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	if (msg.o.io.err < 0) {
		return SET_ERRNO(msg.o.io.err);
	}

	/* Remove mountpoint */
	if (rootfs == 0) {
		msg.type = mtSetAttr;
		msg.i.attr.type = atDev;
		msg.i.attr.oid = oid;
		msg.i.data = &oid;
		msg.i.size = sizeof(oid);

		if (msgSend(oid.port, &msg) < 0) {
			return SET_ERRNO(-EIO);
		}

		return SET_ERRNO(msg.o.attr.err);
	}
	/* TODO: unregister root in kernel */
	else {
		return EOK;
	}
}
