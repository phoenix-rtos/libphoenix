/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * string/strerror (errno names)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>


typedef struct {
	uint16_t errnum;
	uint16_t offset;
} errinfo_t;


static const char errnames[] = {
	#include "errno.str.inc"
};

static const errinfo_t errtab[] = {
	#include "errno.tab.inc"
};

static const char gainames[] = {
	#include "gaierr.str.inc"
};

static const errinfo_t gaitab[] = {
	#include "gaierr.tab.inc"
};

// static assert: 16-bit offset
static const char assert_errnames_size[-(sizeof(errnames) > 0xFFFF)] __attribute__((unused));
static const char assert_gainames_size[-(sizeof(gainames) > 0xFFFF)] __attribute__((unused));
static char err_unknownMsg[32]; /* FIXME: Make thread safe? IMHO overkill */


static int err_cmp(const void *key, const void *elem)
{
	const errinfo_t *p = elem;
	long err = (long)key;

	return err - (long)p->errnum;
}


static inline const char *strerror_(const errinfo_t *tab, size_t tabsz, const char *names, int errnum)
{
	const errinfo_t *e;

	if (errnum < 0) {
		errnum = -errnum;
	}

	e = bsearch((void *)(long)errnum, tab, tabsz / sizeof(*tab), sizeof(*tab), err_cmp);

	if (e == NULL) {
		(void)sprintf(err_unknownMsg, "Unknown error %d", errnum);
		errno = EINVAL;
		return err_unknownMsg;
	}
	else {
		return names + e->offset;
	}
}


char *strerror(int errnum)
{
	return (char *)strerror_(errtab, sizeof(errtab), errnames, errnum);
}


const char *gai_strerror(int errnum)
{
	return strerror_(gaitab, sizeof(gaitab), gainames, errnum);
}
