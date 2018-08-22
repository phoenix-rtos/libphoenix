#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/file.h>


char *ptsname(int fd)
{
	static char namebuf[1024];

	if (ptsname_r(fd, namebuf, sizeof(namebuf)))
		return NULL;

	return namebuf;
}
