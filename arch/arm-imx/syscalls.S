/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (arm-imx)
 *
 * Copyright 2017, 2018 Phoenix Systems
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

/* TODO - fork */

SYSCALLDEF(execle, 5)

SYSCALLDEF(execve, 6)

SYSCALLDEF(exit, 7)

SYSCALLDEF(waitpid, 8)

SYSCALLDEF(getpid, 9)

SYSCALLDEF(getppid, 10)

SYSCALLDEF(beginthreadex, 11)

SYSCALLDEF(endthread, 12)

SYSCALLDEF(usleep, 13)

SYSCALLDEF(mutexCreate, 14)

SYSCALLDEF(mutexLock, 15)

SYSCALLDEF(mutexUnlock, 16)

SYSCALLDEF(semaphoreCreate, 17)

SYSCALLDEF(semaphoreDown, 18)

SYSCALLDEF(semaphoreUp, 19)

SYSCALLDEF(condCreate, 20)

SYSCALLDEF(condWait, 21)

SYSCALLDEF(condSignal, 22)

SYSCALLDEF(resourceDestroy, 23)

SYSCALLDEF(interrupt, 24)

SYSCALLDEF(portCreate, 25)

SYSCALLDEF(portDestroy, 26)

SYSCALLDEF(portRegister, 27)

SYSCALLDEF(msgSend, 28)

SYSCALLDEF(msgRecv, 29)

SYSCALLDEF(msgRespond, 30)

SYSCALLDEF(lookup, 31)

SYSCALLDEF(gettime, 32)

SYSCALLDEF(keepidle, 33)

SYSCALLDEF(mmdump, 34)

SYSCALLDEF(platformctl, 35)

SYSCALLDEF(wdgreload, 36)

SYSCALLDEF(fileAdd, 37)

SYSCALLDEF(fileSet, 38)

SYSCALLDEF(fileGet, 39)

SYSCALLDEF(fileRemove, 40)

SYSCALLDEF(threadsinfo, 41)

SYSCALLDEF(meminfo, 42)

SYSCALLDEF(syspageprog, 43)
