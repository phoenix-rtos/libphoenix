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


#define NULL (void *)0

#define max(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })

#define min(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
        _a > _b ? _b : _a; \
})


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


typedef u32 handle_t;


/* Aliases used by applications */


typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef s64 int64_t;


#define UINT32_MAX 0xffffffff


/* Declarations for built-in functions */


extern void *memcpy(void *dst, const void *src, size_t len);

extern void *memset(void *s, int c, size_t n);

extern void *memmove(void *str1, const void *str2, size_t n);


/* Specific functions for I/O */


static inline u8 inb(void *addr)
{
	u8 b;
	
	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inb %%dx, %%al; \
		movb %%al, %0;" \
	:"=b" (b) \
	:"g" (addr) \
	:"edx", "eax");
	return b;
}


static inline void outb(void *addr, u8 b)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movb %1, %%al; \
		outb %%al, %%dx"
	:
	:"g" (addr), "b" (b)
	:"eax", "edx");

	return;
}


static inline u16 inw(void *addr)
{
	u16 w;

	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inw %%dx, %%ax; \
		movw %%ax, %0;" \
	:"=g" (w) \
	:"g" (addr) \
	:"edx", "eax");

	return w;
}


static inline void outw(void *addr, u16 w)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movw %1, %%ax; \
		outw %%ax, %%dx"
		:
		:"g" (addr), "g" (w)
		:"eax", "edx");

	return;
}


static inline u32 inl(void *addr)
{
	u32 l;
	
	__asm__ volatile
	(" \
		movl %1, %%edx; \
		inl %%dx, %%eax; \
		movl %%eax, %0;" \
		:"=g" (l) \
		:"g" (addr) \
		:"eax", "edx", "memory");

	return l;
}


static inline void outl(void *addr, u32 l)
{
	__asm__ volatile
	(" \
		movl %0, %%edx; \
		movl %1, %%eax; \
		outl %%eax, %%dx"
		:
		:"g" (addr), "g" (l)
		:"eax", "edx");

	return;
}


/*
 * Debug
 */


extern void ph_printf(const char *fmt, ...);


/*
 * Memory management
 */


extern void *ph_mmap(void *vaddr, size_t size, int prot, int flags, oid_t, offs_t offs);


extern int ph_munmap(void *vaddr, size_t size);


extern void *calloc(size_t nmemb, size_t size);


extern void free(void *ptr);


extern void *realloc(void *ptr, size_t size);


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


extern handle_t ph_mutex(void);


extern void ph_lock(handle_t mutex);


extern void ph_unlock(handle_t mutex);


/*
 * Semaphores
 */


extern handle_t ph_semaphore(void);


extern int ph_p(handle_t semaphore);


extern int ph_v(handle_t semaphore);


/*
 * Conditionals
 */


extern handle_t ph_cond(void);


extern int ph_wait(handle_t cont, handle_t mutex, time_t timeout);


extern int ph_signal(handle_t cond);


/*
 * Handler disposal
 */


extern void ph_dispose(unsigned int h);


/*
 * Time management
 */


extern int ph_gettime(void);


#endif
