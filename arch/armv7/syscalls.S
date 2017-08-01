/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (armv7)
 *
 * Copyright 2017 Phoenix Systems
 * Author; Pawel Pisarczyk, Aleksander Kaminski
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
	svc $sn; \
	blx lr;


SYSCALLDEF(debug)
	SYSCALL(0)
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


SYSCALLDEF(execle)
	SYSCALL(4);
.size execle, .-execle


SYSCALLDEF(exit)
	SYSCALL(5);
.size exit, .-exit


SYSCALLDEF(usleep)
	SYSCALL(6);
.size usleep, .-usleep


SYSCALLDEF(beginthread)
	SYSCALL(8);
.size beginthread, .-beginthread


SYSCALLDEF(send)
	SYSCALL(13);
.size send, .-send


SYSCALLDEF(lock)
	SYSCALL(16);
.size lock, .-lock


SYSCALLDEF(unlock)
	SYSCALL(17);
.size unlock, .-unlock
