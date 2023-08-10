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
#include <string.h>
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


static int err_cmp(const void *key, const void *elem)
{
	const errinfo_t *p = elem;
	long err = (long)key;

	return err - (long)p->errnum;
}


static inline const char *strerror_(const errinfo_t *tab, size_t tabsz, const char *names, int errnum, char *buff, size_t bufflen, int *err)
{
	const errinfo_t *e;

	if (errnum < 0) {
		errnum = -errnum;
	}

	e = bsearch((void *)(long)errnum, tab, tabsz / sizeof(*tab), sizeof(*tab), err_cmp);
	if (e == NULL) {
		(void)snprintf(buff, bufflen, "Unknown error %d", errnum);
		*err = EINVAL;
		return buff;
	}

	return names + e->offset;
}


char *strerror(int errnum)
{
	static char unknownMsg[32];
	const char *str;
	int err = 0;

	str = strerror_(errtab, sizeof(errtab), errnames, errnum, unknownMsg, sizeof(unknownMsg), &err);
	if (err != 0) {
		errno = err;
	}

	return (char *)str;
}


int strerror_r(int errnum, char *buf, size_t buflen)
{
	char unknownMsg[32];
	const char *str;
	int ret = 0;
	size_t len;

	str = strerror_(errtab, sizeof(errtab), errnames, errnum, unknownMsg, sizeof(unknownMsg), &ret);
	len = strlen(str);
	if (buflen > len) {
		memcpy(buf, str, len + 1);
	}
	else {
		ret = ERANGE;
	}

	return ret;
}


const char *gai_strerror(int errnum)
{
	static char unknownMsg[32];
	const char *str;
	int err = 0;

	str = strerror_(gaitab, sizeof(gaitab), gainames, errnum, unknownMsg, sizeof(unknownMsg), &err);
	if (err != 0) {
		errno = err;
	}

	return (char *)str;
}
