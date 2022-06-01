/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * wchar.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_WCHAR_H_
#define _LIBPHOENIX_WCHAR_H_

#include <stddef.h>

#define WEOF ((wchar_t)-1)

typedef int wint_t;


int wcscmp(const wchar_t *ws1, const wchar_t *ws2);


#endif /* _LIBPHOENIX_WCHAR_H_ */
