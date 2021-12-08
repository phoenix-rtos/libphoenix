/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/msg.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_MSG_H_
#define _LIBPHOENIX_SYS_MSG_H_

#include <sys/types.h>
#include <phoenix/msg.h>


/* Port management */
extern int portCreate(unsigned int *port);


extern void portDestroy(unsigned int port);


extern int portRegister(unsigned int port, const char *name, oid_t *oid);


extern int lookup(const char *name, oid_t *file, oid_t *dev);


/* Message passing */
extern int msgSend(unsigned int port, msg_t *m);


extern int msgPulse(unsigned int port, msg_t *m);


extern int msgRecv(unsigned int port, msg_t *m, unsigned long int *rid);


extern int msgRespond(unsigned int port, msg_t *m, unsigned long int rid);


#endif
