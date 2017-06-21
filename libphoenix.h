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

#define MSECS_TO_TIME_T(msecs) ((msecs)*((time_t)10e6))
#define SECS_TO_TIME_T(secs)   ((secs)*((time_t)10e9))

typedef struct _oid_t {
	u32 port;
	u64 id;
} oid_t;


typedef u32 handle_t;


typedef enum _msgtype_t {
	MSG_NOTIFY, MSG_NORMAL
} msgtype_t;


typedef enum _msgop_t {
	MSG_READ, MSG_WRITE, MSG_DEVCTL
} msgop_t;


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

extern unsigned int strlen(const char *s);

extern unsigned int strlen(const char *s);

extern int strcmp(const char *s1, const char *s2);

extern int strncmp(const char *s1, const char *s2, unsigned int count);

extern char *strcpy(char *dest, const char *src);

extern unsigned int strtoul(char *nptr, char **endptr, int base);

extern int strtol(char *nptr, char **endptr, int base);

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

/* syscalls numbers */
#define	SYS_debug	0
#define	SYS_mmap	1
#define	SYS_munmap	2
#define	SYS_threadCreate	9
#define	SYS_threadExit	10
#define	SYS_send	12
#define	SYS_recv	13
#define	SYS_respond	14
#define	SYS_mutex	15
#define	SYS_lock	16
#define	SYS_unlock	17
#define	SYS_gettime	23
#define	SYS_sleep	24


extern int ph_syscall(unsigned int number, ...);


/*
 * Debug
 */


extern int ph_printf(const char *fmt, ...);


/*
 * Memory management
 */


#define PROT_READ	0x1
#define PROT_WRITE	0x2
#define PROT_EXEC	0x4
#define PROT_NONE	0x0

#define MAP_SHARED	0x01
#define MAP_PRIVATE	0x02

#define MAP_FIXED	0x10
#define MAP_ANONYMOUS	0x20



extern void *ph_mmap(void *vaddr, size_t size, int prot, int flags, oid_t, offs_t offs);


extern int ph_munmap(void *vaddr, size_t size);


extern void *calloc(size_t nmemb, size_t size);


extern void free(void *ptr);


extern void *realloc(void *ptr, size_t size);


extern void *malloc(size_t size);


/*
 * Process management
 */


extern void ph_vfork(void);


extern void ph_execle(const char *path, ...);


extern int ph_exit(int code);


extern void ph_beginthread(void (*start)(void *), unsigned int priority, void *arg);


extern void ph_exitthread(void);


/*
 * Interrupt management
 */


extern void ph_setintr(unsigned int intr, int (*handler)(void));


/*
 * Message passing
 */


extern int ph_send(u32 port, msgop_t op, void *data, size_t size, msgtype_t type, void *rdata, size_t rsize);


extern int ph_recv(u32 port, void *data, size_t size, msgtype_t *type, msgop_t *op, size_t *rsize, unsigned int *sender);


extern int ph_respond(u32 port, int err, void *data, size_t size);


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


extern time_t ph_gettime(void);


extern void ph_sleep(time_t timeout);


#endif
