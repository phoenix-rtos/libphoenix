/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp/longjmp
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <setjmp.h>


int setjmp(jmp_buf var)
{
	long rv = 0;

	__asm__ volatile ("\
		adr r0, 1f + 1                   ;\
		mov r1, %1                       ;\
		mov r2, sp                       ;\
		stm r1, {r0, r2, r4, r5, r6, r7} ;\
		b 2f                             ;\
		1: mov %0, #1                    ;\
		2: "
		: "+l" (rv)
		: "l" (var)
		: "r0", "r1", "r2", "r3", "cc", "memory");

	return rv;
}


void longjmp(jmp_buf var, int m)
{
	__asm__ volatile ("\
		mov r1, %0                       ;\
		ldm r1, {r0, r2, r4, r5, r6, r7} ;\
		mov sp, r2                       ;\
		bx r0"
		:
		: "l" (var)
		: "r1");

	/* Unreachable */
	for (;;) ;
}
