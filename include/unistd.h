/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Kamil Amanowicz, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_UNISTD_H_
#define _LIBPHOENIX_UNISTD_H_


#include <arch.h>
#include <stddef.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


#define F_OK 0
#define R_OK (1 << 2)
#define W_OK (1 << 1)
#define X_OK (1 << 0)


#define _SC_OPEN_MAX   0
#define _SC_IOV_MAX    1
#define _SC_ATEXIT_MAX 2
#define _SC_CLK_TCK    3
#define _SC_PAGESIZE   4
#define _SC_PAGE_SIZE  _SC_PAGESIZE /* spec. 1170 compatibility */


extern long sysconf(int name);


/* NOTE: Legacy from SUSv2, new applications should use sysconf(_SC_PAGESIZE) */
static inline int getpagesize(void)
{
	return (int)sysconf(_SC_PAGESIZE);
}


extern int vfork(void);


extern pid_t fork(void);


extern int execl(const char *path, const char *arg, ...);


extern int execle(const char *path, const char *arg, ...);


extern int execlp(const char *path, const char *arg, ...);


extern int execve(const char *path, char *const argv[], char *const envp[]);


extern int execvp(const char *file, char *const argv[]);


extern int execvpe(const char *file, char *const argv[], char *const envp[]);


extern int execv(const char *path, char *const argv[]);


extern pid_t getpid(void);


extern pid_t getppid(void);


extern uid_t getuid(void);


extern uid_t geteuid(void);


extern gid_t getgid(void);


extern gid_t getegid(void);


extern int setuid(uid_t uid);


extern int seteuid(uid_t uid);


extern int setgid(gid_t gid);


extern int setegid(gid_t gid);


extern int usleep(useconds_t usecs);


extern unsigned int sleep(unsigned int seconds);


extern int fsync(int fd);


extern void sync(void);


extern int chdir(const char *path);


extern int fchdir(int fd);


extern int fchown(int fd, uid_t owner, gid_t group);


extern int rmdir(const char *path);


extern ssize_t read(int fildes, void *buf, size_t nbyte);


extern ssize_t write(int fildes, const void *buf, size_t nbyte);


extern int dup(int fildes);


extern int isatty(int fildes);


extern int dup2(int fildes, int fildes2);


extern void _exit(int status);


/* Creates a symbolic link named linkpath which contains the string target. If linkpath exists, it will not be overwritten. */
extern int symlink(const char *target, const char *linkpath);


/* Creates a new link (also known as a hard link) to an existing file. If newpath exists, it will not be overwritten. */
extern int link(const char *oldpath, const char *newpath);


extern int unlink(const char *pathname);


extern ssize_t readlink(const char *path, char *buf, size_t bufsiz);


extern int access(const char *pathname, int mode);


extern char *getcwd(char *buf, size_t size);


extern int getopt(int argc, char * const argv[], const char *optstring);


extern int close(int fildes);


extern off_t lseek(int fildes, off_t offset, int whence);


extern int pipe(int fildes[2]);


extern int truncate(const char *path, off_t length);


extern int ftruncate(int fildes, off_t length);


extern unsigned int alarm(unsigned int seconds);


extern int issetugid(void);


extern char *getlogin(void);


extern int getlogin_r(char *name, size_t namesize);


extern char *crypt(const char *key, const char *salt);


extern int setpgid(pid_t pid, pid_t pgid);


extern pid_t getpgid(pid_t pid);


extern int setpgrp(void);


extern pid_t getpgrp(void);


extern pid_t setsid(void);


extern pid_t getsid(pid_t pid);


extern int gethostname(char *name, size_t namelen);


extern int sethostname(const char *name, size_t namelen);


extern char *getpass(const char *prompt);


extern int getgroups(int size, gid_t list[]);


extern char *ttyname(int fildes);


extern int ttyname_r(int fildes, char *name, size_t namesize);


extern int pause(void);


extern char *optarg;


extern int optind, opterr, optopt;


extern char **__environ, **environ;


#ifdef __cplusplus
}
#endif


#endif
