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

#ifndef _LIBPHOENIX_LOCALE_H_
#define _LIBPHOENIX_LOCALE_H_


enum { LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME };


struct lconv
{
	const char *decimal_point;
};


extern const struct lconv _fixed_locale;


extern char* setlocale(int category, const char* locale);


static inline const struct lconv *localeconv (void)
{
	return &_fixed_locale;
}


#endif
