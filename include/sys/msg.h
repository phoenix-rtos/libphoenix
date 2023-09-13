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


#ifdef __cplusplus
extern "C" {
#endif


/* Port management */


extern int portCreate(uint32_t *port);

extern void portDestroy(uint32_t port);

extern int portRegister(uint32_t port, const char *name, oid_t *oid);

extern int lookup(const char *name, oid_t *file, oid_t *dev);


/* Message passing */


extern int msgSend(uint32_t port, msg_t *m);

extern int msgPulse(uint32_t port, msg_t *m);

extern int msgRecv(uint32_t port, msg_t *m, msg_rid_t *rid);

extern int msgRespond(uint32_t port, msg_t *m, msg_rid_t rid);


#ifdef __cplusplus
}
#endif


#endif
