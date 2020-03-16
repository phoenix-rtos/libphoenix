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

#include <stddef.h>
#include <sys/types.h>
#include <phoenix/msg.h>


/* Port management */


extern int portCreate(uint32_t port);

extern void portDestroy(uint32_t port);

extern int portRegister(uint32_t port, const char *name, oid_t *oid);

extern int lookup(const char *name, oid_t *file, oid_t *dev);


/* Message passing */


extern int msgSend(uint32_t port, msg_t *m);

extern int msgPulse(uint32_t port, msg_t *m);

extern int portRecv(int port, msg_t *m);


static inline int msgRecv(uint32_t port, msg_t *m, unsigned int *rid)
{
	int handle = portRecv(port, m);
	if (handle < 0)
		return handle;
	*rid = handle;
	return 0;
}


extern int msgRespond(uint32_t port, int error, msg_t *m, unsigned int rid);


#endif
