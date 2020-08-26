/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * SBI routines (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_RISCV64_IO_H_
#define _LIBPHOENIX_RISCV64_IO_H_

extern void sbi_putchar(int c);

extern int sbi_getchar(void);

static inline unsigned char inb(void *addr)
{
	unsigned char b;
	b = *((char*)addr);
	return b;
}


static inline void outb(void *addr, unsigned char b)
{
	*((char*)addr) = b;
}

#endif
