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
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _LIBPHOENIX_MALLOC_H
#define _LIBPHOENIX_MALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _mallocInfo_t {
	size_t allocsz;
	size_t freesz;
} mallocInfo_t;


void mallocInfo(mallocInfo_t *info);


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_MALLOC_H */
