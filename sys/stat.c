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
#include <limits.h>

#include "posix/utils.h"


/* path needs to be canonical */
static int _stat_abs(const char *path, struct stat *buf)
{
	oid_t oid, dev;
	msg_t msg = {0};
	int err;

	if (lookup(path, &oid, &dev) < 0)
		return -ENOENT;

	memset(buf, 0, sizeof(struct stat));

	buf->st_dev = oid.port;
	buf->st_ino = oid.id;
	buf->st_rdev = dev.port;

	msg.type = mtGetAttr;
	msg.i.attr.oid = oid;

	msg.i.attr.type = atMTime;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_mtime = msg.o.attr.val;

	msg.i.attr.type = atATime;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_atime = msg.o.attr.val;

	msg.i.attr.type = atCTime;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_ctime = msg.o.attr.val;

	msg.i.attr.type = atLinks;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_nlink = msg.o.attr.val;

	msg.i.attr.type = atMode;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_mode = msg.o.attr.val;

	msg.i.attr.type = atUid;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_uid = msg.o.attr.val;

	msg.i.attr.type = atGid;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_gid = msg.o.attr.val;

	msg.i.attr.type = atSize;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_size = msg.o.attr.val;

	msg.i.attr.type = atBlocks;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_blocks = msg.o.attr.val;

	msg.i.attr.type = atIOBlock;
	if (((err = msgSend(oid.port, &msg)) < 0) || ((err = msg.o.attr.err) < 0))
		return err;
	buf->st_blksize = msg.o.attr.val;

	return EOK;
}


int stat(const char *path, struct stat *buf)
{
	char *canonical;
	int ret;

	if ((canonical = resolve_path(path, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	ret = _stat_abs(canonical, buf);

	free(canonical);
	return SET_ERRNO(ret);
}


mode_t umask(mode_t cmask)
{
	/* TODO: add proper umask() implementation */
	/* Don't restrict process file mode mask for now */
	return 0;
}


int lstat(const char *path, struct stat *buf)
{
	char *canonical;
	int ret;

	/* resolve_last_symlink = 0 -> stat on a symlink */
	if ((canonical = resolve_path(path, NULL, 0, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	ret = _stat_abs(canonical, buf);

	free(canonical);
	return SET_ERRNO(ret);
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

	if (path == NULL)
		return SET_ERRNO(-EINVAL);

	/* allow_missing_leaf = 1 -> we will be creating a dir */
	canonical_name = resolve_path(path, NULL, 1, 1);
	if (canonical_name == NULL)
		return -1; /* errno set by resolve_path */

	name = strrchr(canonical_name, '/');

	if (name == canonical_name) {
		name++;
		parent = "/";

		if (*name == 0) { /* special case - "/" */
			free(canonical_name);
			return SET_ERRNO(-EEXIST);
		}
	}
	else {
		*(name++) = 0;
		parent = canonical_name;
	}

	if (strlen(name) > NAME_MAX) {
		free(canonical_name);
		return SET_ERRNO(-ENAMETOOLONG);
	}

	if (lookup(parent, NULL, &dir) < EOK) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}

	msg.type = mtCreate;
	msg.i.create.type = otDir;
	msg.i.create.mode = mode | S_IFDIR;
	msg.i.create.dir = dir;
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dir.port, &msg) < 0) {
		free(canonical_name);
		return SET_ERRNO(-ENOMEM); /* -EIO ? */
	}

	free(canonical_name);
	return SET_ERRNO(msg.o.create.err);
}


extern int sys_chmod(const char *path, mode_t mode);


int chmod(const char *path, mode_t mode)
{
	char *canonical;
	int err;

	/* POSIX: symlinks are to be followed here */
	canonical = resolve_path(path, NULL, 1, 0);
	if (canonical == NULL)
		return -1; /* errno set by resolve_path */

	err = sys_chmod(canonical, mode);
	free(canonical);
	return SET_ERRNO(err);
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

	/* FIXME: add renaming dirs support */

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
