/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mount
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "mount.h"
#include "msg.h"
#include "fs.h"
#include "../string.h"
#include "../errno.h"
#include "../stdlib.h"


int mount(const char *path, unsigned int port)
{
	int err;
	size_t pos, len;
	oid_t srv;
	char pstack[16], *pheap = NULL, *pptr;
	fsmount_t *msg;

	if (path == NULL)
		return -EINVAL;

	len = strlen(path);

	if (len == 1 && path[0] == '/')
		return portRegister(port, path);

	if (len < 2 || path[0] != '/')
		return -EINVAL;

	/* Separate name from path */
	for (pos = len; pos > 0; --pos) {
		if (path[pos - 1] == '/')
			break;
	}

	/* Find server which will mount port */
	if (pos + 1 <= sizeof(pstack)) {
		pptr = pstack;
	}
	else {
		if ((pheap = malloc(pos - 1)) == NULL)
			return -ENOMEM;
		pptr = pheap;
	}

	memcpy(pptr, path, pos);
	pptr[pos] = '\0';

	err = lookup(pptr, &srv);

	free(pheap);

	if (err)
		return err;

	if ((msg = malloc(len + 1 + sizeof(fsmount_t))) == NULL)
		return -ENOMEM;

	msg->port = port;
	strcpy(msg->name, path);

	err = send(srv.port, MOUNT, msg, len + 1 + sizeof(fsmount_t), NORMAL, NULL, 0);

	free(msg);

	return err;
}


int umount(const char *path)
{

}

