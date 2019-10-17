/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/io.h
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_IO_H_
#define _LIBPHOENIX_SYS_IO_H_

#include <arch.h>

#ifdef __ARCH_SYS_IO
#include __ARCH_SYS_IO
#else
#error "sys/io.h is not defined for current architectue!"
#endif

#endif
