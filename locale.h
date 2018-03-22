/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * locale.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_LOCALE_H
#define _LIBPHOENIX_LOCALE_H


struct lconv
{
	const char *decimal_point;
};


extern const struct lconv _fixed_locale;


static inline const struct lconv *localeconv (void)
{
	return &_fixed_locale;
}


#endif /* _LIBPHOENIX_LOCALE_H */
