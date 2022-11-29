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


int mount(const char *source, const char *target, const char *fstype, unsigned long mode, const char *data)
{
	struct stat buf;
	oid_t toid, soid, doid;
	msg_t msg = {0};
	char *source_abspath, *target_abspath;
	int err;

	mount_i_msg_t *imnt = (mount_i_msg_t *)msg.i.raw;
	mount_o_msg_t *omnt = (mount_o_msg_t *)msg.o.raw;

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
	imnt->dev = soid;
	imnt->mnt = toid;
	imnt->mode = mode;
	strncpy(imnt->fstype, fstype, sizeof(imnt->fstype));
	imnt->fstype[sizeof(imnt->fstype) - 1] = '\0';

	msg.i.size = data != NULL ? strlen(data) : 0;
	msg.i.data = (char *)data; /* FIXME: dropping const because of broken msg_t declaration */

	if (msgSend(soid.port, &msg) < 0)
		return SET_ERRNO(-EIO);

	/* Mount failed */
	if (omnt->err < 0) {
		return SET_ERRNO(omnt->err);
	}
	doid = omnt->oid;

	/* Create mountpoint */
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
	mount_o_msg_t *omnt = (mount_o_msg_t *)msg.o.raw;
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
		msg.type = mtMountPoint;
		msg.i.data = &dev;
		msg.i.size = sizeof(dev);

		if (msgSend(dev.port, &msg) < 0) {
			return SET_ERRNO(-EIO);
		}

		if (omnt->err < 0) {
			/* Check for rootfs */
			if (omnt->err == -ENOENT) {
				rootfs = 1;
			}
			/* No mountpoint */
			else {
				return SET_ERRNO(omnt->err);
			}
		}
		else {
			oid = omnt->oid;
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
