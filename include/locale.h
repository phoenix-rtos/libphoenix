/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * locale.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_LOCALE_H
#define _LIBPHOENIX_LOCALE_H


#ifdef __cplusplus
extern "C" {
#endif


enum { LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME };


struct lconv
{
	const char *decimal_point;
};


extern const struct lconv _fixed_locale;


char* setlocale(int category, const char* locale);


static inline const struct lconv *localeconv (void)
{
	return &_fixed_locale;
}


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_LOCALE_H */
