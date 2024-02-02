/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat.h
 *
 * Copyright 2018, 2024 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz, Aleksander Kaminski, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/types.h>
#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif


#include <phoenix/posix-stat.h>


#define st_atime st_atim.tv_sec
#define st_ctime st_ctim.tv_sec
#define st_mtime st_mtim.tv_sec


extern int lstat(const char *path, struct stat *buf);


extern int stat(const char *path, struct stat *buf);


extern int fstat(int fildes, struct stat *buf);


extern mode_t umask(mode_t cmask);


extern int mkdir(const char *pathname, mode_t mode);


extern int mkfifo(const char *pathname, mode_t mode);


extern int mknod(const char *pathname, mode_t mode, dev_t dev);


extern int chmod(const char *path, mode_t mode);
extern int fchmod(int fd, mode_t mode);


extern int chown(const char *path, uid_t owner, gid_t group);
extern int lchown(const char *path, uid_t owner, gid_t group);


#ifdef __cplusplus
}
#endif


#endif
