/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mman.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_MMAN_H_
#define _LIBPHOENIX_SYS_MMAN_H_

#include <sys/types.h>
#include <phoenix/mman.h>
#include <phoenix/sysinfo.h>


#define MAP_ANON   MAP_ANONYMOUS
#define MAP_FAILED (void *)-1


extern void mmdump(void);


extern void meminfo(meminfo_t *info);


extern int syspageprog(syspageprog_t *prog, int index);


extern void *mmap(void *vaddr, size_t size, int prot, int flags, oid_t *oid, off_t offs);


extern int munmap(void *vaddr, size_t size);


extern addr_t va2pa(void *va);


#endif
