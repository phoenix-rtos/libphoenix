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


SYSCALLDEF(waitpid)
	SYSCALL(6);
.size waitpid, .-waitpid


SYSCALLDEF(getpid)
	SYSCALL(7);
.size getpid, .-getpid


SYSCALLDEF(getppid)
	SYSCALL(8);
.size getppid, .-getppid


SYSCALLDEF(beginthread)
	SYSCALL(9);
.size beginthread, .-beginthread


SYSCALLDEF(endthread)
	SYSCALL(10);
.size endthread, .-endthread


SYSCALLDEF(usleep)
	SYSCALL(11);
.size usleep, .-usleep


SYSCALLDEF(mutexCreate)
	SYSCALL(12);
.size mutexCreate, .-mutexCreate


SYSCALLDEF(mutexLock)
	SYSCALL(13);
.size mutexLock, .-mutexLock


SYSCALLDEF(mutexUnlock)
	SYSCALL(14);
.size mutexUnlock, .-mutexUnlock


SYSCALLDEF(semaphoreCreate)
	SYSCALL(15);
.size semaphoreCreate, .-semaphoreCreate


SYSCALLDEF(semaphoreDown)
	SYSCALL(16);
.size semaphoreDown, .-semaphoreDown


SYSCALLDEF(semaphoreUp)
	SYSCALL(17);
.size semaphoreUp, .-semaphoreUp


SYSCALLDEF(condCreate)
	SYSCALL(18);
.size condCreate, .-condCreate


SYSCALLDEF(condWait)
	SYSCALL(19);
.size condWait, .-condWait


SYSCALLDEF(condSignal)
	SYSCALL(20);
.size condSignal, .-condSignal


SYSCALLDEF(resourceDestroy)
	SYSCALL(21);
.size resourceDestroy, .-resourceDestroy


SYSCALLDEF(interrupt)
	SYSCALL(22);
.size interrupt, .-interrupt


SYSCALLDEF(portCreate)
	SYSCALL(23);
.size portCreate, .-portCreate


SYSCALLDEF(portDestroy)
	SYSCALL(24);
.size portDestroy, .-portDestroy


SYSCALLDEF(portRegister)
	SYSCALL(25);
.size portRegister, .-portRegister


SYSCALLDEF(send)
	SYSCALL(26);
.size send, .-send


SYSCALLDEF(recv)
	SYSCALL(27);
.size recv, .-recv


SYSCALLDEF(respond)
	SYSCALL(28);
.size respond, .-respond


SYSCALLDEF(lookup)
	SYSCALL(29);
.size lookup, .-lookup


SYSCALLDEF(gettime)
	SYSCALL(30);
.size gettime, .-gettime


SYSCALLDEF(keepidle)
	SYSCALL(31)
.size keepidle, .-keepidle


SYSCALLDEF(mmdump)
	SYSCALL(32)
.size mmdump, .-mmdump


SYSCALLDEF(platformctl)
	SYSCALL(33)
.size platformctl, .-platformctl
