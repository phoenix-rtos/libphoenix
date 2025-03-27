/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * dlfcn.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_DLFCN_H_
#define _LIBPHOENIX_DLFCN_H_


#ifdef __cplusplus
extern "C" {
#endif


#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 0x100
#define RTLD_LOCAL  0x200


typedef struct {
	const char *dli_fname; /* Pathname of mapped object file. */
	void *dli_fbase;       /* Base of mapped address range. */
	const char *dli_sname; /* Symbol name or null pointer. */
	void *dli_saddr;       /* Symbol address or null pointer. */
} Dl_info_t;


int dladdr(const void *restrict, Dl_info_t *restrict);
int dlclose(void *);
char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *restrict, const char *restrict);


#ifdef __cplusplus
}
#endif


#endif
