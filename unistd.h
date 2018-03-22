/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_UNISTD_H_
#define _LIBPHOENIX_UNISTD_H_

#include <sys/types.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define _SC_CLK_TCK 0

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

enum {
	F_OK = 0,
	R_OK = 1 << 0,
	W_OK = 1 << 1,
	X_OK = 1 << 2,
};


static inline int getpagesize(void)
{
	return SIZE_PAGE;
}


extern int vfork(void);


extern pid_t fork(void);


extern int execle(const char *path, const char *arg, ...);


extern int execve(const char *path, char *const argv[], char *const envp[]);


extern int execv(const char *path, char *const argv[]);


extern pid_t getpid(void);


extern pid_t getppid(void);


extern int usleep(useconds_t usecs);


extern unsigned int sleep(unsigned int seconds);


extern int chdir(const char *path);


extern ssize_t read(int fildes, void *buf, size_t nbyte);


extern ssize_t write(int fildes, const void *buf, size_t nbyte);


extern int dup(int fildes);


extern int dup2(int fildes, int fildes2);


extern void _exit(int status);


extern int symlink(const char *path1, const char *path2);


extern int link(const char *path1, const char *path2);


extern int unlink(const char *pathname);

extern int access(const char *pathname, int mode);

extern char *getcwd(char *buf, size_t size);


extern int getopt(int argc, char * const argv[], const char *optstring);


extern int close(int fildes);


extern off_t lseek(int fildes, off_t offset, int whence);


extern int ftruncate(int fildes, off_t length);


extern char *optarg;


extern int optind, opterr, optopt;


#endif
