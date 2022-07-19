/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * I/O functions
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_IA32_IO_H_
#define _LIBPHOENIX_IA32_IO_H_


#ifdef __cplusplus
extern "C" {
#endif


static inline unsigned char inb(void *addr)
{
	unsigned char b;

	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inb %%dx, %%al; \
		movb %%al, %0;" \
	:"=b" (b) \
	:"g" (addr) \
	:"edx", "eax");
	return b;
}


static inline void outb(void *addr, unsigned char b)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movb %1, %%al; \
		outb %%al, %%dx"
	:
	:"g" (addr), "b" (b)
	:"eax", "edx");

	return;
}


static inline unsigned short inw(void *addr)
{
	unsigned short w;

	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inw %%dx, %%ax; \
		movw %%ax, %0;" \
	:"=g" (w) \
	:"g" (addr) \
	:"edx", "eax");

	return w;
}


static inline void outw(void *addr, unsigned short w)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movw %1, %%ax; \
		outw %%ax, %%dx"
		:
		:"g" (addr), "g" (w)
		:"eax", "edx");

	return;
}


static inline unsigned int inl(void *addr)
{
	unsigned int l;

	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inl %%dx, %%eax; \
		movl %%eax, %0;" \
		:"=g" (l) \
		:"g" (addr) \
		:"eax", "edx", "memory");

	return l;
}


static inline void outl(void *addr, unsigned int l)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movl %1, %%eax; \
		outl %%eax, %%dx"
		:
		:"g" (addr), "g" (l)
		:"eax", "edx");

	return;
}


#ifdef __cplusplus
}
#endif


#endif
