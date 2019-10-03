/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdio_ext.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDIO_EXT_H_
#define _LIBPHOENIX_STDIO_EXT_H_


#include <stdio.h>


/* Discard all pending buffered I/O. */
extern void __fpurge (FILE *__fp) __THROW;


#endif
