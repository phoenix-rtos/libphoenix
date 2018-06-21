/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sockport.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SOCKPORT_H_
#define _SYS_SOCKPORT_H_

#include <sys/msg.h>
#include <sys/sockdefs.h>
#include <stdint.h>
#include <time.h>


enum {
	sockmSocket = 0x50c30000, sockmShutdown, sockmPoll,
	sockmConnect, sockmBind, sockmListen, sockmAccept,
	sockmSend, sockmRecv, sockmGetSockName, sockmGetPeerName,
	sockmGetFl, sockmSetFl, sockmGetOpt, sockmSetOpt,
};

enum { MAX_SOCKNAME_LEN = sizeof(((msg_t *)0)->o.raw) - 2 * sizeof(size_t) };


#define PATH_SOCKSRV "/dev/netsocket"


typedef union sockport_msg_ {
	struct {
		int domain, type, protocol;
	} socket;
	struct {
		int backlog;
	} listen;
	struct {
		int level;
		int optname;
	} opt;
	struct {
		int flags;
		size_t addrlen;
		char addr[MAX_SOCKNAME_LEN];
	} send;
	struct {
		int events;
		time_t timeout;
	} poll;
} sockport_msg_t;


typedef struct sockport_resp_ {
	ssize_t ret;
	union {
		struct {
			size_t addrlen;
			char addr[MAX_SOCKNAME_LEN];
		} sockname;
	};
} sockport_resp_t;


#endif
