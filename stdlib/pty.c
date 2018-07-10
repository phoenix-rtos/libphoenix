#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/file.h>

#include "posixsrv/posixsrv.h"


int grantpt(int fd)
{
	return 0;
}


int unlockpt(int fd)
{
	msg_t msg = { 0 };
	oid_t oid;
	posixsrv_devctl_t ctl;

	if (fileGet(fd, 1, &oid, NULL, NULL) < 0)
		return -1;

	msg.type = mtDevCtl;
	ctl.id = oid.id;
	ctl.pty.type = pxUnlockpt;

	memcpy(msg.i.raw, &ctl, sizeof(ctl));

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	return 0;
}


char *ptsname(int fd)
{
	return NULL;
}


int ptsname_r(int fd, char *buf, size_t buflen)
{
	return 0;
}
