/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for file operations)
 *
 * Copyright 2017-2019 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk, Kamil Amanowicz, Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "posix/utils.h"


extern int sys_openat(int dirfd, const char *filename, int oflag, mode_t mode);
extern int fileLink(int fildes, const char *path, int dirfd,  const char *name, int flags);
extern int fileUnlink(int dirfd, const char *path, int flags);
extern int fileSeek (int fildes, off_t *offset, int whence);

extern int sys_fcntl(int fd, int cmd, long val);
extern int sys_pipe(int fildes[2], int flags);
extern int sys_dup3(int fildes, int fildes2, int flags);

extern ssize_t fileRead(int fd, void *buf, size_t nbyte, off_t *offset);
extern ssize_t fileWrite(int fd, const void *buf, size_t nbyte, off_t *offset);

WRAP_ERRNO_DEF(int, close, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, ftruncate, (int fildes, off_t length), (fildes, length))


int read(int fildes, void *buf, size_t nbyte)
{
	return SET_ERRNO(fileRead(fildes, buf, nbyte, NULL));
}


int pread(int fildes, void *buf, size_t nbyte, off_t offset)
{
	return SET_ERRNO(fileRead(fildes, buf, nbyte, &offset));
}


int write(int fildes, const void *buf, size_t nbyte)
{
	return SET_ERRNO(fileWrite(fildes, buf, nbyte, NULL));
}


int pwrite(int fildes, const void *buf, size_t nbyte, off_t offset)
{
	return SET_ERRNO(fileWrite(fildes, buf, nbyte, &offset));
}


int dup3(int fildes, int fildes2, int flags)
{
	int err;
	while ((err = sys_dup3(fildes, fildes2, flags)) == -EINTR)
		;
	return SET_ERRNO(err);
}


int dup(int fildes)
{
	return fcntl(fildes, F_DUPFD, 0);
}


int dup2(int fildes, int fildes2)
{
	int err;
	if ((err = sys_dup3(fildes, fildes2, 0)) == -EINVAL && fildes == fildes2)
		return fildes2;
	return SET_ERRNO(err);
}


int pipe(int fildes[2])
{
	int err;
	while ((err = sys_pipe(fildes, 0)) == -EINTR)
		;
	return SET_ERRNO(err);
}


int linkat(int fildes, const char *path, int dirfd, const char *name, int flags)
{
	return SET_ERRNO(fileLink(fildes, path, dirfd, name, flags));
}


int link(const char *path1, const char *path2)
{
	return linkat(AT_FDCWD, path1, AT_FDCWD, path2, 0);
}


int unlinkat(int dirfd, const char *path, int flags)
{
	return SET_ERRNO(fileUnlink(dirfd, path, flags));
}


int unlink(const char *path)
{
	return unlinkat(AT_FDCWD, path, 0);
}


static int vopenat(int dirfd, const char *path, int oflag, va_list ap) {
	mode_t mode = 0;
	int err;

	if (oflag & O_CREAT)
		mode = va_arg(ap, mode_t);

	do
		err = sys_openat(dirfd, path, oflag, mode);
	while (err == -EINTR);

	return SET_ERRNO(err);
}


int openat(int dirfd, const char *path, int oflag, ...)
{
	int ret;
	va_list ap;

	va_start(ap, oflag);
	ret = vopenat(dirfd, path, oflag, ap);
	va_end(ap);

	return ret;
}


int open(const char *path, int oflag, ...)
{
	int ret;
	va_list ap;

	va_start(ap, oflag);
	ret = vopenat(AT_FDCWD, path, oflag, ap);
	va_end(ap);

	return ret;
}


int creat(const char *path, int mode)
{
	return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}


int symlinkat(const char *path1, int dirfd, const char *path2)
{
	return SET_ERRNO(-ENOSYS);
}


int symlink(const char *path1, const char *path2)
{
	return symlinkat(path1, AT_FDCWD, path2);
}


int access(const char *path, int amode)
{
	int fd;
	int result;

	/* NOTE: for now checking only if file exists
	 *   nonblocking against fifos */
	if ((fd = open(path, O_NONBLOCK)) != -1) {
		close(fd);
		result = 0;
	}
	else if (errno == EAGAIN) {
		result = 0;
	}
	else {
		result = -1;
	}

	return result;
}


off_t lseek(int fd, off_t offset, int whence)
{
	int result;
	if ((result = fileSeek(fd, &offset, whence)) < 0)
		return SET_ERRNO(result);
	return offset;
}


extern int deviceCreate(int cwd, const char *, int portfd, id_t id, mode_t mode);


int create_dev(int portfd, id_t id, const char *path, mode_t mode)
{
	if (!(mode & (S_IFCHR | S_IFBLK)))
		return -EINVAL;
	return deviceCreate(AT_FDCWD, path, portfd, id, (mode & (S_IFCHR | S_IFBLK)) | 0500);
}


int fcntl(int fd, int cmd, ...)
{
	va_list ap;
	long val;

	/* FIXME: handle varargs properly */
	va_start(ap, cmd);
	val = va_arg(ap, unsigned);
	va_end(ap);

	return SET_ERRNO(sys_fcntl(fd, cmd, val));
}


int grantpt(int fd)
{
	return EOK;
}


int unlockpt(int fd)
{
	int lock = 0;
	return ioctl(fd, TIOCSPTLCK, &lock);
}


int ptsname_r(int fd, char *buf, size_t buflen)
{
	int id, len;

	while (ioctl(fd, TIOCGPTN, &id) < 0) {
		if (errno != EINTR)
			return -1;
	}

	len = snprintf(buf, buflen, "/dev/pts/%d", id);

	if (len >= buflen)
		return SET_ERRNO(-ERANGE);

	return 0;
}


int isatty(int fd)
{
/* FIXME remove after moving ioctls from devctls */
#ifdef NOMMU
	if (fd == 1 || fd == 0)
		return 1;

	return 0;
#endif

	struct termios t;
	return tcgetattr(fd, &t) == 0;
}
