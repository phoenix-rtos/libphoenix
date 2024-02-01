/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * strings.h
 *
 * Copyright 2017, 2018, 2024 Phoenix Systems
 * Author: Kamil Amanowicz, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STRINGS_H_
#define _LIBPHOENIX_STRINGS_H_


#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Compares the string pointed to, by str1 to the string pointed to by str2 case-insensitively. */
extern int strcasecmp(const char *str1, const char *str2);


/* Compares at most the first n bytes of str1 and str2 case-insensitively. */
extern int strncasecmp(const char *s1, const char *s2, size_t n);


/* Find first set bit */
int ffs(int i);


#ifdef __cplusplus
}
#endif


#endif
