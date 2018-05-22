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

#include ARCH

#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/resource.h>


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


int socket(int domain, int type, int protocol)
{
	return 0;
}


int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	return 0;
}


int listen(int socket, int backlog)
{
	return 0;
}


ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
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
