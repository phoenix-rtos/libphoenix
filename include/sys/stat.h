/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <time.h>
#include <phoenix/stat.h>


extern int lstat(const char *path, struct stat *buf);


extern int stat(const char *path, struct stat *buf);


extern int fstat(int fildes, struct stat *buf);


extern int fstatat(int dirfd, const char *path, struct stat *buf, int flags);


extern mode_t umask(mode_t cmask);


extern int mkdir(const char *pathname, mode_t mode);
extern int mkdirat(int dirfd, const char *pathname, mode_t mode);


extern int mkfifo(const char *pathname, mode_t mode);
extern int mkfifoat(int dirfd, const char *pathname, mode_t mode);


extern int mknod(const char *pathname, mode_t mode, dev_t dev);
extern int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev);


extern int chmod(const char *path, mode_t mode);
extern int fchmod(int fd, mode_t mode);


extern int chown(const char *path, uid_t owner, gid_t group);
extern int lchown(const char *path, uid_t owner, gid_t group);

#endif
