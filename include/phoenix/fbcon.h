/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fbcon.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Adam Greloch
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FBCON_H_
#define _LIBPHOENIX_FBCON_H_


#include <sys/ioctl.h>


#ifdef __cplusplus
extern "C" {
#endif

/* ioctls */
#define FBCONSETMODE _IOV('t', 0x80, int)
#define FBCONGETMODE _IO('t', 0x81)


#define FBCON_ENABLED     0x0
#define FBCON_DISABLED    0x1
#define FBCON_UNSUPPORTED 0x2


#ifdef __cplusplus
}
#endif


#endif
