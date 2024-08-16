/*
 * Phoenix-RTOS
 *
 * libphoenix/rtld
 *
 * stubs.c
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <dlfcn.h>
#include <sys/debug.h>
#include <stdlib.h>


/* As dynamic linker is not added as a NEEDED dependency in libphoenix, stub implementation are required.
 * They are overridden by dynamic linkers functions in runtime.  */

__attribute__((weak)) int dladdr(const void *restrict a, Dl_info_t *restrict b)
{
	return 0;
}


__attribute__((weak)) int dlclose(void *a)
{
	return -1;
}


__attribute__((weak)) char *dlerror(void)
{
	return "dlfcn: not supported in statically linked binaries";
}


__attribute__((weak)) void *dlopen(const char *a, int b)
{
	return NULL;
}


__attribute__((weak)) void *dlsym(void *restrict a, const char *restrict b)
{
	return NULL;
}
