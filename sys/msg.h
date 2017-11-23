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
	NOTIFY = 0, NORMAL
} msgtype_t;


typedef enum _msgop_t {
	READ = 0, WRITE, DEVCTL, OPEN, CLOSE
} msgop_t;


typedef struct _msghdr_t {
	msgtype_t type;
	msgop_t op;
	size_t rsize;
	unsigned int sender;
} msghdr_t;


typedef struct {
	size_t offset;
	void *buff;
	size_t size;
} msgdata_t;


extern int portCreate(u32 *port);


extern void portDestroy(u32 port);


extern int portRegister(u32 port, char *name);


extern int send(u32 port, msgop_t op, void *data, size_t size, msgtype_t type, void *rdata, size_t rsize);


extern int recv(u32 port, void *data, size_t size, msghdr_t *hdr);


extern int respond(u32 port, int err, void *data, size_t size);


extern int lookup(char *name, unsigned int *port);


#endif
