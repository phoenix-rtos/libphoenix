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


#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned int jmp_buf[34];
typedef jmp_buf sigjmp_buf;


extern int _setjmp(jmp_buf var);


extern void _longjmp(jmp_buf var, int m) __attribute__((__noreturn__));


extern int setjmp(jmp_buf var);


__attribute__((__noreturn__)) static inline void longjmp(jmp_buf var, int m)
{
	_longjmp(var, m);
}


extern int sigsetjmp(sigjmp_buf env, int savesigs);


__attribute__((__noreturn__)) static inline void siglongjmp(sigjmp_buf env, int val)
{
	_longjmp(env, val);
}


#ifdef __cplusplus
}
#endif


#endif
