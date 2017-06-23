/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/msg
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_MSG_H_
#define _LIBPHOENIX_MSG_H_

#include ARCH


typedef enum _msgtype_t {
	NOTIFY, NORMAL
} msgtype_t;


typedef enum _msgop_t {
	READ, WRITE, DEVCTL
} msgop_t;


extern int send(u32 port, msgop_t op, void *data, size_t size, msgtype_t type, void *rdata, size_t rsize);


extern int recv(u32 port, void *data, size_t size, msgtype_t *type, msgop_t *op, size_t *rsize, unsigned int *sender);


extern int respond(u32 port, int err, void *data, size_t size);


#endif
