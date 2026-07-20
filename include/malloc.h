/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * malloc.h
 *
 * Copyright 2026 Phoenix Systems
 * Author: Michal Lach
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_MALLOC_H_
#define _LIBPHOENIX_MALLOC_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _mallocInfo_t {
	/*
	 * Amount of memory mapped from the system for user-data and dynamic
	 * allocation structures
	 */
	size_t mapsz;

	/*
	 * Memory free to be used by the user, does not include allocator
	 * structures overhead
	 */
	size_t freesz;

	/* Maximum allocation size throughout allocators lifetime */
	size_t maxalloc;
} mallocInfo_t;


void mallocInfo(mallocInfo_t *info);


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_MALLOC_H_ */
