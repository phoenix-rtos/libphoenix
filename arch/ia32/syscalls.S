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
	movl $sn, %eax; \
	int $0x80; \
	ret;


SYSCALLDEF(debug)
	SYSCALL(0);
.size debug, .-debug


SYSCALLDEF(mmap)
	SYSCALL(1);
.size mmap, .-mmap


SYSCALLDEF(munmap)
	SYSCALL(2);
.size munmap, .-munmap


SYSCALLDEF(vfork)
	SYSCALL(3);
.size vfork, .-vfork


SYSCALLDEF(beginthread)
	SYSCALL(8);
.size beginthread, .-beginthread


SYSCALLDEF(lock)
	SYSCALL(16);
.size lock, .-lock


SYSCALLDEF(unlock)
	SYSCALL(17);
.size unlock, .-unlock


SYSCALLDEF(exit)
	SYSCALL(9);
.size exit, .-exit
