/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SETJMP_H_
#define _LIBPHOENIX_SETJMP_H_


typedef unsigned int jmp_buf[6];


typedef int sigjmp_buf;


extern int setjmp(jmp_buf var);


extern void longjmp(jmp_buf var, int m);

#endif
