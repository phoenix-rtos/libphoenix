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
#define __THROW	__attribute__ ((__nothrow__ __LEAF))
#define __LEAF , __leaf__
#define __attribute_pure__ __attribute__ ((__pure__))
#define __nonnull(params) __attribute__ ((__nonnull__ params))

typedef int wint_t;
typedef struct
{
  int __count;
  union
  {
    __WINT_TYPE__ __wch;
    char __wchb[4];
  } __value;		/* Value so far.  */
} __mbstate_t;
typedef __mbstate_t mbstate_t;

extern int wcscmp(const wchar_t *ws1, const wchar_t *ws2);

/* Write multibyte character representation of wide character string
   SRC to DST.  */

extern size_t wcsrtombs (char *__restrict __dst,
			 const wchar_t **__restrict __src, size_t __len,
			 mbstate_t *__restrict __ps) __THROW;

/* Compare S1 and S2.  */
// extern int wcscmp(const wchar_t *s1, const wchar_t *s2);
// extern int wcscmp (const wchar_t *__s1, const wchar_t *__s2)
//      __THROW __attribute_pure__ __nonnull ((1, 2));

/* Return the number of wide characters in S.  */
extern size_t wcslen (const wchar_t *__s) __THROW __attribute_pure__;

#endif /* _LIBPHOENIX_WCHAR_H_ */
