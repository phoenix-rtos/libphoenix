/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "posix/utils.h"


int stat(const char *path, struct stat *buf)
{
	oid_t oid, dev;
	msg_t msg = {0};
	char *canonical_name = canonicalize_file_name(path);

	if (lookup(canonical_name, &oid, &dev) < 0) {
		free(canonical_name);
		errno = ENOENT;
		return -1;
	}

	free(canonical_name);

	memset(buf, 0, sizeof(struct stat));

	buf->st_dev = oid.port;
	buf->st_ino = oid.id;
	buf->st_rdev = dev.port;

	msg.type = mtGetAttr;
	msg.i.attr.oid = oid;
	msg.i.attr.val = 0;

	msg.i.attr.type = atMTime;
	if (!msgSend(oid.port, &msg))
		buf->st_mtime = msg.o.attr.val;

	msg.i.attr.type = atATime;
	if (!msgSend(oid.port, &msg))
		buf->st_atime = msg.o.attr.val;

	msg.i.attr.type = atCTime;
	if (!msgSend(oid.port, &msg))
		buf->st_ctime = msg.o.attr.val;

	msg.i.attr.type = atLinks;
	if (!msgSend(oid.port, &msg))
		buf->st_nlink = msg.o.attr.val;

	msg.i.attr.type = atMode;
	if (!msgSend(oid.port, &msg))
		buf->st_mode = msg.o.attr.val;

	msg.i.attr.type = atUid;
	if (!msgSend(oid.port, &msg))
		buf->st_uid = msg.o.attr.val;

	msg.i.attr.type = atGid;
	if (!msgSend(oid.port, &msg))
		buf->st_gid = msg.o.attr.val;

	msg.i.attr.type = atSize;
	if (!msgSend(oid.port, &msg))
		buf->st_size = msg.o.attr.val;

	return EOK;
}


mode_t umask(mode_t cmask)
{
	return -ENOSYS;
}


int lstat(const char *path, struct stat *buf)
{
	return stat(path, buf);
}


int creat(const char *pathname, int mode)
{
	return open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
}


int mkdir(const char *path, mode_t mode)
{
	msg_t msg = { 0 };
	oid_t dir;
	char *canonical_name, *name, *parent;

	if (path == NULL) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	canonical_name = canonicalize_file_name(path);
	name = strrchr(canonical_name, '/');

	if (name == canonical_name) {
		name++;
		parent = "/";
	}
	else {
		*(name++) = 0;
		parent = canonical_name;
	}

	if (lookup(parent, NULL, &dir) < EOK) {
		free(canonical_name);
		SET_ERRNO(ENOENT);
		return -1;
	}

	msg.type = mtCreate;
	msg.i.create.type = otDir;
	msg.i.create.mode = mode | S_IFDIR;
	msg.i.create.dir = dir;
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dir.port, &msg) < 0) {
		free(canonical_name);
		SET_ERRNO(ENOMEM);
		return -1;
	}

	free(canonical_name);
	SET_ERRNO(msg.o.create.err);
	return !msg.o.create.err ? 0 : -1;
}


extern int sys_chmod(const char *path, mode_t mode);


int chmod(const char *path, mode_t mode)
{
	char *canonical;
	int err;
	canonical = canonicalize_file_name(path);
	err = sys_chmod(canonical, mode);
	free(canonical);
	return err;
}


int lchown(const char *path, uid_t owner, gid_t group)
{
	return 0;
}


int mknod(const char *path, mode_t mode, dev_t dev)
{
	if (S_ISFIFO(mode))
		return mkfifo(path, mode);
	else
		return -EINVAL;
}


int rename(const char *old, const char *new)
{
	int err;

	if ((err = link(old, new)) < 0)
		return err;

	if ((err = unlink(old)) < 0)
		return err;

	return 0;
}


int chown(const char *path, uid_t owner, gid_t group)
{
	return 0;
}
