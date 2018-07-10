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


time_t mktime(struct tm *timeptr)
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


int inet_aton(const char *cp, struct in_addr *addr)
{
	return -1;
}


int inet_ntoa(const char *cp, struct in_addr *addr)
{
	return -1;
}


char *ctime(const time_t *clock)
{
	return NULL;
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


struct tm *localtime_r(const time_t *timep, struct tm *result)
{
	return NULL;
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


FILE *popen(const char *command, const char *type)
{
	return NULL;
}


int pclose(FILE *stream)
{
	return 0;
}
