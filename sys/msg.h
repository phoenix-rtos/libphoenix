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
#include "../phoenix-rtos-kernel/include/msg.h"


/* Port management */


extern int portCreate(u32 *port);


extern void portDestroy(u32 port);


extern int portRegister(u32 port, char *name, oid_t *oid);


extern int lookup(char *name, unsigned int *port);


/* Message passing */


extern int msgSend(u32 port, msg_t *m);


extern int msgPulse(u32 port, msg_t *m);


extern int msgRecv(u32 port, msg_t *m, unsigned int *rid);


extern int msgRespond(u32 port, unsigned int rid);


#endif
