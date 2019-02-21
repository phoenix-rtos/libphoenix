/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/if_nametoindex.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

unsigned int if_nametoindex(const char *ifname)
{
	int fd, res, len;
	struct ifreq ifr = { 0 };

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd <= 0)
		return 0;

	len = strlen(ifname);
	if (len > IFNAMSIZ)
		len = IFNAMSIZ;
	memcpy(&ifr.ifr_name, ifname, len);

	res = ioctl(fd, SIOCGIFINDEX, &ifr);

	close(fd);
	return res < 0 ? 0 : ifr.ifr_ifindex;
}
