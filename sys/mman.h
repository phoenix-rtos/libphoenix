/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mman
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#include ARCH
#include "../../phoenix-rtos-kernel/include/sysinfo.h"
#include "../../phoenix-rtos-kernel/include/mman.h"


/* Predefined oids */
#define OID_NULL         NULL
#define OID_PHYSMEM      (void *)-1


extern void mmdump(void);


extern void meminfo(meminfo_t *info);


extern void *mmap(void *vaddr, size_t size, int prot, int flags, oid_t *oid, offs_t offs);


extern int munmap(void *vaddr, size_t size);


#endif
