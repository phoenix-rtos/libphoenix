/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SOCKDEFS_H_
#define _SYS_SOCKDEFS_H_

#define SOCK_NONBLOCK 0x8000
#define SOCK_CLOEXEC  0x4000

#define SOL_IPV6 41  /* IPPROTO_IPV6 */
#define SOL_RAW  255 /* IPPROTO_RAW */

#endif
