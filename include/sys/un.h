/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/un.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _SYS_UN_H_
#define _SYS_UN_H_

#include <sys/socket.h>


#define SUN_LEN(addr) ((socklen_t)(((struct sockaddr_un *)0)->sun_path) + strlen((addr)->sun_path))


struct sockaddr_un {
	sa_family_t sun_family;
	char        sun_path[108];
};


#endif
