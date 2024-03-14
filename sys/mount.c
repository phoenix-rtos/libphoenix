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
	char *target_abspath = resolve_path(target, NULL, 1, 0);
	if (target_abspath == NULL) {
		return -1; /* errno set by resolve_path */
	}
	char *source_abspath = resolve_path(source, NULL, 1, 0);
	if (source_abspath == NULL) {
		free(target_abspath);
		return -1; /* errno set by resolve_path */
	}

	oid_t toid, soid, doid;

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

	struct stat buf;
	int err = stat(target_abspath, &buf);

	free(target_abspath);
	free(source_abspath);

	if (err < 0) {
		return err;
	}

	if (!S_ISDIR(buf.st_mode)) {
		return SET_ERRNO(-ENOTDIR);
	}

	msg_t msg = {
		.type = mtMount,
		.i.size = (data != NULL) ? strlen(data) : 0,
		.i.data = data,
		.oid = soid
	};

	mount_i_msg_t *imnt = (mount_i_msg_t *)msg.i.raw;
	mount_o_msg_t *omnt = (mount_o_msg_t *)msg.o.raw;

	imnt->mnt = toid;
	imnt->mode = mode;
	strncpy(imnt->fstype, fstype, sizeof(imnt->fstype));
	imnt->fstype[sizeof(imnt->fstype) - 1] = '\0';


	if (msgSend(soid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	/* Mount failed */
	if (msg.o.err < 0) {
		return SET_ERRNO(msg.o.err);
	}
	doid = omnt->oid;

	/* Create mountpoint */
	memset(&msg, 0, sizeof(msg));

	msg.type = mtSetAttr;
	msg.oid = toid;
	msg.i.attr.type = atDev;
	msg.i.data = &doid;
	msg.i.size = sizeof(oid_t);

	if (msgSend(toid.port, &msg) < 0) {
		return SET_ERRNO(-EIO); /* FIXME: rollback partial mount */
	}

	return SET_ERRNO(msg.o.err);
}


int umount(const char *target)
{
	int rootfs = 0;
	oid_t oid, dev;

	char *abspath = resolve_path(target, NULL, 1, 0);
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
	msg_t msg = {
		.type = mtGetAttr,
		.oid = oid,
		.i.attr.type = atMode,
	};

	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	if (msg.o.err < 0) {
		return SET_ERRNO(msg.o.err);
	}

	/* Got mountpoint */
	if (S_ISDIR(msg.o.attr.val)) {
		/* TODO: implement umount() by mountpoint, set ENOTSUP errno for now */
		return SET_ERRNO(-ENOTSUP);
	}
	/* Got mounted device */
	/* TODO: check device type (should be S_IFBLK?) */
	else {
		mount_o_msg_t *omnt = (mount_o_msg_t *)msg.o.raw;

		/* Get mountpoint */
		msg.type = mtMountPoint;
		msg.oid = dev;

		if (msgSend(dev.port, &msg) < 0) {
			return SET_ERRNO(-EIO);
		}

		if (msg.o.err < 0) {
			/* Check for rootfs */
			if (msg.o.err == -ENOENT) {
				rootfs = 1;
			}
			/* No mountpoint */
			else {
				return SET_ERRNO(msg.o.err);
			}
		}
		else {
			oid = omnt->oid;
		}
	}

	/* Unmount filesystem */
	msg.type = mtUmount;
	msg.oid = dev;

	if (msgSend(dev.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	if (msg.o.err < 0) {
		return SET_ERRNO(msg.o.err);
	}

	/* Remove mountpoint */
	if (rootfs == 0) {
		msg.type = mtSetAttr;
		msg.oid = oid;
		msg.i.attr.type = atDev;
		msg.i.data = &oid;
		msg.i.size = sizeof(oid);

		if (msgSend(oid.port, &msg) < 0) {
			return SET_ERRNO(-EIO);
		}

		return SET_ERRNO(msg.o.err);
	}
	/* TODO: unregister root in kernel */
	else {
		return EOK;
	}
}
