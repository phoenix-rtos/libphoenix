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

#include <arch.h>
#include <phoenix/msg.h>


/* Port management */


extern int portCreate(u32 *port);

extern void portDestroy(u32 port);

extern int portRegister(u32 port, const char *name, oid_t *oid);

extern int lookup(const char *name, oid_t *file, oid_t *dev);


/* Message passing */


extern int msgSend(u32 port, msg_t *m);

extern int msgPulse(u32 port, msg_t *m);

extern int msgRecv(u32 port, msg_t *m, unsigned int *rid);

extern int msgRespond(u32 port, msg_t *m, unsigned int rid);


#endif
