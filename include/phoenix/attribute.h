/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * phoenix/attribute.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PHOENIX_ATTRIBUTE_H_
#define _LIBPHOENIX_PHOENIX_ATTRIBUTE_H_


#include <sys/msg.h>

/* Initializes `struct _attrAll` errors to `err` */
void _phoenix_initAttrsStruct(struct _attrAll *attrs, int err);


#endif
