/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Assembly implementation of string functions
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stddef.h>


void *memcpy(void *to, const void *from, size_t n)
{
	__asm__ volatile
	(" \
		cld; \
		movl %0, %%ecx; \
		movl %%ecx, %%edx; \
		andl $3, %%edx; \
		shrl $2, %%ecx; \
		movl %1, %%edi; \
		movl %2, %%esi; \
		rep; movsl; \
		movl %%edx, %%ecx; \
		rep; movsb"
	:
	: "g" (n), "g" (to), "g" (from)
	: "ecx", "edx", "esi", "edi", "cc");

	return to;
}


void *memset(void *where, int v, size_t n)
{
	__asm__ volatile
	(" \
		cld; \
		movl %0, %%ecx; \
		movl %%ecx, %%edx; \
		andl $3, %%edx; \
		shrl $2, %%ecx; \
		\
		xorl %%eax, %%eax; \
		movb %1, %%al; \
		movl %%eax, %%ebx; \
		shll $8, %%ebx; \
		orl %%ebx, %%eax; \
		movl %%eax, %%ebx; \
		shll $16, %%ebx; \
		orl %%ebx, %%eax; \
		\
		movl %2, %%edi; \
		rep; stosl; \
		movl %%edx, %%ecx; \
		rep; stosb"
	: "+d" (n)
	: "m" (v), "m" (where)
	: "eax", "ebx", "cc", "ecx", "edi" ,"memory");

	return where;
}
