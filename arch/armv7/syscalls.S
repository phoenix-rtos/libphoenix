/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (armv7)
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
.type sym, %function; \
sym:


#define SYSCALL(sn) \
	nop;


SYSCALLDEF(debug)
	SYSCALL(0)
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


SYSCALLDEF(lock)
	SYSCALL(9);
.size lock, .-lock


SYSCALLDEF(unlock)
	SYSCALL(10);
.size unlock, .-unlock
