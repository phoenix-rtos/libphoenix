/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * wchar.h
 *
 * Copyright 2018, 2022 Phoenix Systems
 * Author: Michał Mirosław, Damian Loewnau
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


typedef struct
{
	int count;
	union {
		wint_t wch;
		unsigned char wchb[4];
	} value;
} mbstate_t;


extern int wcscmp(const wchar_t *ws1, const wchar_t *ws2);


/* TODO: missing function definition */
extern size_t wcslen(const wchar_t *ws);


/* TODO: missing function definition */
extern size_t wcstombs(char *restrict s, const wchar_t *restrict pwcs, size_t n);


/* TODO: missing function definition */
extern size_t wcsrtombs(char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps);

#endif /* _LIBPHOENIX_WCHAR_H_ */
