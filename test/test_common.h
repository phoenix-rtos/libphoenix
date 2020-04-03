/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_strftime.c
 *
 * Copyright 2020 Phoenix Systems
 * Author: Marcin Brzykcy
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static int test_verbosity;

static inline void tm_to_str(struct tm *t, char *buff)
{
	sprintf(buff, "tm_sec %d, tm_min %d, tm_hour %d, tm_mday %d\n"
			"tm_mon %d tm_year %d tm_wday %d tm_yday %d tm_isdst %d",
			t->tm_sec, t->tm_min, t->tm_hour, t->tm_mday, t->tm_mon, t->tm_year,
			t->tm_wday, t->tm_yday, t->tm_isdst);

}


static inline void init_tm(struct tm *t, const int *input)
{
	t->tm_sec = input[0];
	t->tm_min = input[1];
	t->tm_hour = input[2];
	t->tm_mday = input[3];
	t->tm_mon = input[4];
	t->tm_year = input[5];
	t->tm_wday = input[6];
	t->tm_yday = input[7];
	t->tm_isdst = input[8];
}


static inline int compare_tm(struct tm *t1, struct tm *t2)
{
	return ((t1->tm_sec != t2->tm_sec) || (t1->tm_min != t2->tm_min) || (t1->tm_hour != t2->tm_hour) ||
		   (t1->tm_mday != t2->tm_mday) || (t1->tm_mon != t2->tm_mon) || (t1->tm_year != t2->tm_year) ||
		   (t1->tm_wday != t2->tm_wday) || (t1->tm_yday != t2->tm_yday) || (t1->tm_isdst != t2->tm_isdst));
}


static inline void save_env(void)
{
	char *c = getenv("VERBOSE_TEST");

	if (c != NULL && c[0] >= '0' && c[0] <= '9')
		test_verbosity = (c[0] - '0');
	else
		test_verbosity = 0;
}


static inline int verbose_test(void)
{
	return test_verbosity;
}

#endif
