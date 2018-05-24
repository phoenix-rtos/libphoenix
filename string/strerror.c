/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/strerror (errno names)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <stdint.h>


typedef struct {
	uint16_t errno;
	uint16_t offset;
} errinfo_t;


static const char errnames[] = {
	#include "errstr.inc"
};

static const errinfo_t errtab[] = {
	#include "errtab.inc"
};

// static assert: 16-bit offset
static const char assert_errnames_size[-(sizeof(errnames) > 0xFFFF)] __attribute__((unused));


static int err_cmp(const void *key, const void *elem)
{
	const errinfo_t *p = elem;
	int err = (int)key;

	return err - p->errno;
}


const char *strerror(int errnum)
{
	const errinfo_t *e;

	if (errnum < 0)
		errnum = -errnum;

	e = bsearch((void *)errnum, errtab, sizeof(errtab) / sizeof(*errtab), sizeof(*errtab), err_cmp);

	return e != NULL ? errnames + e->offset : NULL;
}
