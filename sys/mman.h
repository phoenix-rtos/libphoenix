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


#define PROT_READ   0
#define PROT_WRITE  1


#define MAP_PRIVATE    1
#define MAP_FIXED      2
#define MAP_ANONYMOUS  4


/* Predefined oids */
#define OID_NULL         NULL
#define OID_PHYSMEM      (void *)-1


extern void mmdump(void);


extern void *mmap(void *vaddr, size_t size, int prot, int flags, oid_t *oid, offs_t offs);


extern int munmap(void *vaddr, size_t size);


#endif
