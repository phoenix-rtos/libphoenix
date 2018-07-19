/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - stubs
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>


char* setlocale(int category, const char* locale)
{
	return NULL;
}


int fsync(int fd)
{
	return 0;
}


int fchmod(int fd, mode_t mode)
{
	return 0;
}


int fchown(int fd, uid_t owner, gid_t group)
{
	return 0;
}


char *mkdtemp(char *template)
{
	return NULL;
}


int mkstemp(char *template)
{
	return 0;
}


int setgid(gid_t gid)
{
	return 0;
}


int setuid(uid_t uid)
{
	return 0;
}


int setegid(gid_t gid)
{
	return 0;
}


int seteuid(uid_t uid)
{
	return 0;
}


int fchdir(int fildes)
{
	return 0;
}


char *ttyname(int fildes)
{
	return NULL;
}


int ttyname_r(int fildes, char *name, size_t namesize)
{
	return 0;
}


size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const struct tm *restrict timeptr)
{
	return 0;
}


int chroot(const char *path)
{
	return 0;
}


pid_t setsid(void)
{
	return 0;
}


int setpriority(int which, id_t who, int prio)
{
    return 0;
}


int getrlimit(int resource, struct rlimit *rlp)
{
	return 0;
}


int setrlimit(int resource, const struct rlimit *rlp)
{
	return 0;
}


pid_t tcgetpgrp(int fd)
{
	return -1;
}


int tcsetpgrp(int fd, pid_t pgrp)
{
	return -1;
}


void openlog(const char *ident, int option, int facility)
{
}


void syslog(int priority, const char *format, ...)
{
}


void closelog(void)
{
}


void vsyslog(int priority, const char *format, va_list ap)
{
}


void setbuf(FILE *stream, char *buf)
{
}


int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	return 0;
}


struct group *getgrgid(gid_t gid)
{
	return NULL;
}


dev_t makedev(unsigned int maj, unsigned int min)
{
	return 0;
}


int getrusage(int who, struct rusage *usage)
{
	return 0;
}


int reboot(int cmd)
{
	return 0;
}


int kill(pid_t pid, int sig)
{
	return 0;
}


char *strsep (char **string_ptr, const char *delimiter)
{
	return NULL;
}


void sync(void)
{
}


unsigned int major(int dev)
{
	return 0;
}


unsigned int minor(int dev)
{
	return 0;
}
