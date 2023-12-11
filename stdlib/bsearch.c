/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Binary search
 *
 * Copyright 2012 Phoenix Systems
 * Author: Pawel Kolodziej, Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 *
 */

#include <stdlib.h>


void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	ssize_t l = 0, r = nmemb - 1, m;
	int cmp;

	if (nmemb == 0)
		return NULL;

	while (l <= r) {
		m = (l + r) / 2;

		cmp = compar(key, base + m * size);

		if (cmp == 0)
			return (void *)base + m * size;

		if (cmp > 0)
			l = m + 1;
		else
			r = m - 1;
	}
	return NULL;
}
