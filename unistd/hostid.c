/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd: get/sethostid()
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>


#include "file-internal.h"


#define HOSTIDFILE "/etc/hostid"


int sethostid(long hostid)
{
	/* Check if we have rootfs mounted */
	int ret = access("/etc", F_OK);
	if (ret < 0) {
		return -1;
	}

	/* To be compatible with both 32 and 64 bit systems */
	int32_t hostid32 = (int32_t)hostid;
	if (hostid32 != hostid) {
		errno = EOVERFLOW;
		return -1;
	}

	int fd = __safe_open(HOSTIDFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
		return -1;
	}

	ret = __safe_write(fd, &hostid32, sizeof(hostid32));

	__safe_close(fd);

	return (ret != sizeof(hostid32)) ? SET_ERRNO(-EIO) : 0;
}


long gethostid(void)
{
	int fd = __safe_open(HOSTIDFILE, O_RDONLY, DEFFILEMODE);
	if (fd < 0) {
		if (errno == ENOENT) {
			/* No rootfs/hostid file, return arbitrary value */
			return 0;
		}
		return -1;
	}

	int32_t hostid;
	int ret = __safe_read(fd, &hostid, sizeof(hostid));

	__safe_close(fd);

	return (ret != sizeof(hostid)) ? SET_ERRNO(-EIO) : (long)hostid;
}
