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

#ifndef _LIBPHOENIX_ARCH_IA32_STRING_H_
#define _LIBPHOENIX_ARCH_IA32_STRING_H_


#define MEMCPY
static inline void memcpy(void *to, const void *from, unsigned int n)
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
}


#define MEMSET
static inline void memset(void *where, u8 v, unsigned int n)
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
}


#endif
