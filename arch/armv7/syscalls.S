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


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, %function; \
sym: \
	svc $sn; \
	blx lr; \
.size sym, .-sym


SYSCALLDEF(debug, 0)

SYSCALLDEF(mmap, 1)

SYSCALLDEF(munmap, 2)

SYSCALLDEF(vfork, 3)

SYSCALLDEF(execle, 4)

SYSCALLDEF(exit, 5)

SYSCALLDEF(waitpid, 6)

SYSCALLDEF(getpid, 7)

SYSCALLDEF(getppid, 8)

SYSCALLDEF(beginthreadex, 9)

SYSCALLDEF(endthread, 10)

SYSCALLDEF(usleep, 11)

SYSCALLDEF(mutexCreate, 12)

SYSCALLDEF(mutexLock, 13)

SYSCALLDEF(mutexUnlock, 14)

SYSCALLDEF(semaphoreCreate, 15)

SYSCALLDEF(semaphoreDown, 16)

SYSCALLDEF(semaphoreUp, 17)

SYSCALLDEF(condCreate, 18)

SYSCALLDEF(condWait, 19)

SYSCALLDEF(condSignal, 20)

SYSCALLDEF(resourceDestroy, 21)

SYSCALLDEF(interrupt, 22)

SYSCALLDEF(portCreate, 23)

SYSCALLDEF(portDestroy, 24)

SYSCALLDEF(portRegister, 25)

SYSCALLDEF(msgSend, 26)

SYSCALLDEF(msgRecv, 27)

SYSCALLDEF(msgRespond, 28)

SYSCALLDEF(lookup, 29)

SYSCALLDEF(gettime, 30)

SYSCALLDEF(keepidle, 31)

SYSCALLDEF(mmdump, 32)

SYSCALLDEF(platformctl, 33)

SYSCALLDEF(wdgreload, 34)

SYSCALLDEF(fileAdd, 35)

SYSCALLDEF(fileSet, 36)

SYSCALLDEF(fileGet, 37)

SYSCALLDEF(fileRemove, 38)
