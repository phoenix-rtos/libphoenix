/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (ia32)
 *
 * Copyright 2017 Phoenix Systems
 * Author; Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text


#define SYSCALLDEF(sym) \
.globl sym; \
.type sym, @function; \
.align 4, 0x90; \
sym:


#define SYSCALL(sn) \
	movl sn, %eax; \
	int $0x80;


SYSCALLDEF(debug)
	SYSCALL(0);
.size debug, .-debug


SYSCALLDEF(mmap)
	SYSCALL(1);
.size mmap, .-mmap


SYSCALLDEF(munmap)
	SYSCALL(2);
.size munmap, .-munmap


SYSCALLDEF(beginthread)
	SYSCALL(8);
.size beginthread, .-beginthread