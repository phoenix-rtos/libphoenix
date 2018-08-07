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
typedef jmp_buf sigjmp_buf;


extern int setjmp(jmp_buf var);


extern void longjmp(jmp_buf var, int m);


static inline int sigsetjmp(sigjmp_buf env, int savesigs)
{
	return setjmp(env);
}

static inline void siglongjmp(sigjmp_buf env, int val)
{
	longjmp(env, val);
}


#endif
