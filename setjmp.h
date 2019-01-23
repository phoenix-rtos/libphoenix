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


typedef unsigned int jmp_buf[12];
typedef jmp_buf sigjmp_buf;


extern int _setjmp(jmp_buf var);


extern void _longjmp(jmp_buf var, int m);


extern int setjmp(jmp_buf var);


static inline longjmp(jmp_buf var, int m)
{
	_longjmp(var, m);
}


extern int sigsetjmp(sigjmp_buf env, int savesigs);


static inline void siglongjmp(sigjmp_buf env, int val)
{
	_longjmp(env, val);
}


#endif
