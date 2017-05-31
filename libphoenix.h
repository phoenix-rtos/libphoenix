/*
 * Phoenix-RTOS
 *
 * Microkernel API
 *
 * API declaration
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_LIBPHOENIX_H_
#define _LIBPHOENIX_LIBPHOENIX_H_

#include "../phoenix-rtos-kernel/include/errno.h"


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef u32 addr_t;
typedef u64 cycles_t;

typedef u64 usec_t;
typedef s64 offs_t;

typedef unsigned int size_t;
typedef unsigned long long time_t;

typedef struct _oid_t {
	u32 port;
	u64 id;
} oid_t;



/*
 * Debug
 */


extern void ph_printf(const char *fmt, ...);


/*
 * Memory management
 */


extern void *ph_mmap(void *vaddr, size_t size, int prot, int flags, oid_t, offs_t offs);


extern int ph_munmap(void *vaddr, size_t size);


/*
 * Process management
 */


extern void ph_vfork(void);


extern void ph_execle(const char *path, ...);


extern int ph_exit(int code);


extern void ph_beginthread(void *arg, void *stack);


extern void ph_exitthread(void);


/*
 * Interrupt management
 */


extern void ph_setintr(unsigned int intr, int (*handler)(void));


/*
 * Message passing
 */


extern void ph_send(void *ctx);


extern void ph_recv(void *ctx);


/*
 * Mutexes
 */


extern void ph_mutex(void *ctx);


extern void ph_lock(void *ctx);


extern void ph_unlock(void *ctx);


/*
 * Semaphores
 */


extern void ph_semaphore(void *ctx);


extern void ph_p(void *ctx);


extern void ph_v(void *ctx);


/*
 * Conditionals
 */


extern void ph_cond(void *ctx);


extern void ph_wait(void);


extern void ph_signal(void);


/*
 * Handler disposal
 */


extern void ph_dispose(unsigned int h);


/*
 * Time management
 */


extern int ph_gettime(void);


#endif
