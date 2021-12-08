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

#ifndef _LIBPHOENIX_ARCH_IA32_IO_H_
#define _LIBPHOENIX_ARCH_IA32_IO_H_


static inline unsigned char inb(void *addr)
{
	unsigned char b;

	__asm__ volatile(
		"inb %1, %0; "
	: "=a" (b)
	: "Nd" (addr));

	return b;
}


static inline void outb(void *addr, unsigned char b)
{
	__asm__ volatile(
		"outb %0, %1; "
	:: "a" (b), "Nd" (addr));
}


static inline unsigned short inw(void *addr)
{
	unsigned short w;

	__asm__ volatile(
		"inw %1, %0; "
	: "=a" (w)
	: "Nd" (addr));

	return w;
}


static inline void outw(void *addr, unsigned short w)
{
	__asm__ volatile(
		"outw %0, %1; "
	:: "a" (w), "Nd" (addr));
}


static inline unsigned int inl(void *addr)
{
	unsigned int l;

	__asm__ volatile(
		"inl %1, %0; "
	: "=a" (l)
	: "Nd" (addr));

	return l;
}


static inline void outl(void *addr, unsigned int l)
{
	__asm__ volatile(
		"outl %0, %1; "
	:: "a" (l), "Nd" (addr));
}


#endif
