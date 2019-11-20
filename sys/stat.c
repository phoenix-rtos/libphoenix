/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat
 *
 * Copyright 2018-2019 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz, Andrzej Glowinski
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


extern int fifoCreate(int dirfd, const char *filename, mode_t mode);
extern int fileStat(int dirfd, const char *path, struct stat *stat, int flags);
extern int sys_chmod(const char *path, mode_t mode);
extern int deviceCreate(int dirfd, const char *name, int portfd, id_t id, mode_t mode);

int fstatat(int dirfd, const char *path, struct stat *buf, int flags)
{
	return SET_ERRNO(fileStat(dirfd, path, buf, flags));
}


int stat(const char *path, struct stat *buf)
{
	return fstatat(AT_FDCWD, path, buf, 0);
}


/* FIXME AT_SYMLINK_NOFOLLOW should be use instead */
int lstat(const char *path, struct stat *buf)
{
	return fstatat(AT_FDCWD, path, buf, O_NOFOLLOW);
}


int fstat(int fd, struct stat *buf)
{
	return fstatat(fd, NULL, buf, 0);
}


mode_t umask(mode_t cmask)
{
	return -ENOSYS;
}


int mkdirat(int dirfd, const char *path, mode_t mode)
{
	int fd;
	if ((fd = openat(dirfd, path, O_CREAT | O_EXCL | O_DIRECTORY, mode | S_IFDIR)) != -1)
		close(fd);

	return fd < 0 ? -1 : 0;
}


int mkdir(const char *path, mode_t mode)
{
	return mkdirat(AT_FDCWD, path, mode);
}


int chmod(const char *path, mode_t mode)
{
	return SET_ERRNO(-ENOSYS);
}


int lchown(const char *path, uid_t owner, gid_t group)
{
	return 0;
}


int mkfifoat(int dirfd, const char *filename, mode_t mode)
{
	int err;

	while ((err = fifoCreate(dirfd, filename, mode)) == -EINTR)
		;
	return SET_ERRNO(err);
}


int mkfifo(const char *filename, mode_t mode)
{
	return mkfifoat(AT_FDCWD, filename, mode);
}


int mknodat(int dirfd, const char *path, mode_t mode, dev_t dev)
{
	if (S_ISFIFO(mode)) {
		return mkfifoat(dirfd, path, mode);
	}
	else {
		return deviceCreate(dirfd, path, -1, dev, mode);
	}
}


int mknod(const char *path, mode_t mode, dev_t dev)
{
	return mknodat(AT_FDCWD, path, mode, dev);
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
