/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * uuid.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Damian Loewnau
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ERR_H_
#define _LIBPHOENIX_ERR_H_

typedef unsigned char uuid_t[16];

extern void uuid_generate(uuid_t out);
extern void uuid_unparse(const uuid_t uu, char *out);

#endif /* _LIBPHOENIX_ERR_H_ */
