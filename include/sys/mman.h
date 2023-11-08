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

#include <sys/types.h>
#include <phoenix/sysinfo.h>
#include <phoenix/mman.h>


#define MAP_ANON MAP_ANONYMOUS


#ifdef __cplusplus
extern "C" {
#endif


extern void mmdump(void);


extern void meminfo(meminfo_t *info);


extern int syspageprog(syspageprog_t *prog, int index);


extern void *mmap(void *vaddr, size_t size, int prot, int flags, int fildes, off_t offs);


extern int munmap(void *vaddr, size_t size);


extern int mprotect(void *vaddr, size_t len, int prot);


extern addr_t va2pa(void *va);


#ifdef __cplusplus
}
#endif


#endif
