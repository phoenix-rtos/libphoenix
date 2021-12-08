/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/if_indextoname.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


char *if_indextoname(unsigned int ifindex, char *ifname)
{
	struct ifreq ifr = {0};
	int fd, res;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd <= 0)
		return NULL;

	ifr.ifr_ifindex = ifindex;
	res = ioctl(fd, SIOCGIFNAME, &ifr);

	close(fd);
	if (res < 0) {
		return NULL;
	}
	else {
		strncpy(ifname, ifr.ifr_name, IFNAMSIZ);
		ifname[IFNAMSIZ - 1] = '\0';

		return ifname;
	}
}
