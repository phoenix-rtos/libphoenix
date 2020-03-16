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


/* Predefined oids */
#define OID_NULL _Pragma("GCC warning \"'OID_NULL' is deprecated. Use MAP_ANONYMOUS flag and set fd to -1.\"") (-1)
#define OID_PHYSMEM _Pragma("GCC warning \"'OID_PHYSMEM' is deprecated. Use FD_PHYSMEM.\"") (FD_PHYSMEM)


#define MAP_ANON	MAP_ANONYMOUS


#define MAP_FAILED     ((void *)-1)


extern void mmdump(void);


extern void meminfo(meminfo_t *info);


extern int syspageprog(syspageprog_t *prog, int index);


extern void *mmap(void *vaddr, size_t size, int prot, int flags, int fd, off_t offs);


extern int munmap(void *vaddr, size_t size);


extern addr_t va2pa(void *va);


#endif
