#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>


int posix_openpt(int flags)
{
	int fd = open("/dev/ptmx", flags);
	if (fd < 0) {
		return -1;
	}
	return fd;
}


char *ptsname(int fd)
{
	static char namebuf[1024];

	if (ptsname_r(fd, namebuf, sizeof(namebuf)))
		return NULL;

	return namebuf;
}
