/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Binary search
 *
 * Copyright 2012 Phoenix Systems
 * Author: Paweł Kołodziej, Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 *
 */

#include <stdint.h>
#include <stdlib.h>


void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	ssize_t m, l = 0, r = nmemb - 1;
	int cmp;

	if (nmemb == 0)
		return NULL;

	while (l <= r) {
		m = (l + r) / 2;
		cmp = compar(key, (void *)((uintptr_t)base + m * size));

		if (cmp > 0)
			l = m + 1;
		else if (cmp < 0)
			r = m - 1;
		else
			return (void *)((uintptr_t)base + m * size);
	}

	return NULL;
}
