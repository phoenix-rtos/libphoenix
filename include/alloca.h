/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * alloca.h
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ALLOCA_H_
#define _LIBPHOENIX_ALLOCA_H_

#include <stddef.h>


#undef alloca

extern void *alloca(size_t size);


#ifdef __GNUC__
#define alloca(size) __builtin_alloca (size)
#endif


#endif
