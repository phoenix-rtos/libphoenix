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

#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <grp.h>


char* setlocale(int category, const char* locale)
{
	if (category != LC_ALL)
		return NULL;

	if (strcmp(locale, "POSIX") && strcmp(locale, "C"))
		return NULL;

	return "POSIX";
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


int chroot(const char *path)
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


struct group *getgrnam(const char *name)
{
	return NULL;
}


int initgroups(const char *user, gid_t group)
{
	return 0;
}


int flock(int fd, int operation)
{
	return 0;
}
