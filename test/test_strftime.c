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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_common.h"

#define BUFF_LEN 35
#define T1_LEN 37

struct test_data {
	const struct tm *t;
	const char format[BUFF_LEN];
	size_t n;
	const char output[BUFF_LEN];
	/* if ret = 0 expect 0 at output, otherwise expect strlen(output) */
	int ret;
};

static const struct tm t1 = { .tm_sec = 1, .tm_min = 1, .tm_hour = 6, .tm_mday = 2, .tm_mon = 2,
							  .tm_year = 111, .tm_wday = 0, .tm_yday = 2, .tm_isdst = 0 };

static const struct tm t2 = { .tm_sec = 11, .tm_min = 12, .tm_hour = 13, .tm_mday = 23, .tm_mon = 11,
							  .tm_year = 95, .tm_wday = 6, .tm_yday = 235, .tm_isdst = 0 };

static const struct tm t3 = { .tm_sec = 11, .tm_min = 12, .tm_hour = 13, .tm_mday = 23, .tm_mon = 11,
							  .tm_year = 105, .tm_wday = 1, .tm_yday = 235, .tm_isdst = 0 };

static const struct test_data test_vector[T1_LEN] = {
	{ .t = &t1, .format = "%A", .n = 5, .output = "", .ret = 0 },
	{ .t = &t1, .format = "%A", .n = 6, .output = "", .ret = 0 },
	{ .t = &t1, .format = "%A", .n = 7, .output = "Sunday", .ret = 1 },
	{ .t = &t1, .format = "%a %A %b %B", .n = BUFF_LEN, .output = "Sun Sunday Mar March", .ret = 1 },
	{ .t = &t1, .format = "%h", .n = BUFF_LEN, .output = "Mar", .ret = 1 },
	{ .t = &t1, .format = "lorem ipsum %a", .n = BUFF_LEN, .output = "lorem ipsum Sun", .ret = 1 },
	{ .t = &t1, .format = "%i %a", .n = BUFF_LEN, .output = "%i Sun", .ret = 1 },
	{ .t = &t1, .format = "lorem %i ips%aum", .n = BUFF_LEN, .output = "lorem %i ipsSunum", .ret = 1 },
	{ .t = &t2, .format = "%6b%12B", .n = BUFF_LEN, .output = "   Dec    December", .ret = 1 },
	{ .t = &t2, .format = "%6a%12A", .n = BUFF_LEN, .output = "   Sat    Saturday", .ret = 1 },
	{ .t = &t3, .format = "%a %A %b %B", .n = BUFF_LEN, .output = "Mon Monday Dec December", .ret = 1 },
	{ .t = &t1, .format = "%c", .n = BUFF_LEN, .output = "Sun Mar  2 06:01:01 2011", .ret = 1 },
	{ .t = &t1, .format = "%C %6C %07C %1C", .n = BUFF_LEN, .output = "20 000020 0000020 20", .ret = 1 },
	{ .t = &t2, .format = "%C %6C %07C", .n = BUFF_LEN, .output = "19 000019 0000019", .ret = 1 },
	{ .t = &t3, .format = "%6d %3d %2d %d", .n = BUFF_LEN, .output = "000023 023 23 23", .ret = 1 },
	{ .t = &t1, .format = "%6d %3d %2d %d", .n = BUFF_LEN, .output = "000002 002 02 02", .ret = 1 },
	{ .t = &t1, .format = "%D %12D %012D", .n = BUFF_LEN, .output = "03/02/11     03/02/11 000003/02/11", .ret = 1 },
	{ .t = &t1, .format = "%e %6e %06e %1e", .n = BUFF_LEN, .output = " 2      2 000002  2", .ret = 1 },
	{ .t = &t2, .format = "%e %6e %06e %1e", .n = BUFF_LEN, .output = "23     23 000023 23", .ret = 1 },
	{ .t = &t2, .format = "%5F %15F", .n = BUFF_LEN, .output = "1995-12-23      1995-12-23", .ret = 1 },
	{ .t = &t2, .format = "%F %015F", .n = BUFF_LEN, .output = "1995-12-23 000001995-12-23", .ret = 1 },
	{ .t = &t1, .format = "%g %05g %G %07G", .n = BUFF_LEN, .output = "10 00010 2010 0002010", .ret = 1 },
	{ .t = &t2, .format = "%H %04H %I %04I", .n = BUFF_LEN, .output = "13 0013 01 0001", .ret = 1 },
	{ .t = &t2, .format = "%M %05M %p", .n = BUFF_LEN, .output = "12 00012 PM", .ret = 1 },
	{ .t = &t1, .format = "%M %05M %p", .n = BUFF_LEN, .output = "01 00001 AM", .ret = 1 },
	{ .t = &t1, .format = "%p%5p %r", .n = BUFF_LEN, .output = "AM   AM 06:01:01 AM", .ret = 1 },
	{ .t = &t2, .format = "%p%5p %r", .n = BUFF_LEN, .output = "PM   PM 01:12:11 PM", .ret = 1 },
	{ .t = &t2, .format = "%20r", .n = BUFF_LEN, .output = "         01:12:11 PM", .ret = 1 },
	{ .t = &t2, .format = "%g", .n = BUFF_LEN, .output = "95", .ret = 1 },
	{ .t = &t2, .format = "%g %05g %G %07G", .n = BUFF_LEN, .output = "95 00095 1995 0001995", .ret = 1 },
	{ .t = &t2, .format = "%R %R %S %05S", .n = BUFF_LEN, .output = "13:12 13:12 11 00011", .ret = 1 },
	{ .t = &t2, .format = "%T %15T", .n = BUFF_LEN, .output = "13:12:11        13:12:11", .ret = 1 },
	{ .t = &t2, .format = "%u %05u %w %05w", .n = BUFF_LEN, .output = "6 00006 6 00006", .ret = 1 },
	{ .t = &t2, .format = "%U %05U %W %05W", .n = BUFF_LEN, .output = "33 00033 33 00033", .ret = 1 },
	{ .t = &t3, .format = "%u %05u %U %5U %V %5V", .n = BUFF_LEN, .output = "1 00001 34 00034 35 00035", .ret = 1 },
	{ .t = &t3, .format = "%x %X", .n = BUFF_LEN, .output = "12/23/05 13:12:11", .ret = 1 },
	{ .t = &t2, .format = "%y %05y %Y %05Y", .n = BUFF_LEN, .output = "95 00095 1995 01995", .ret = 1 }
};


int strftime_assert(const struct test_data *data)
{
	char buff[BUFF_LEN];
	int ret = strftime(buff, data->n, data->format, data->t);
	if (ret == 0 && data->ret == 0) {
		return 0;
	}
	if (strcmp(buff, data->output) || ret != strlen(data->output)) {
		if (verbose_test()) {
			printf("Format string \"%s\"\nOutput string \"%s\"\n", data->format, buff);
			printf("Expected      \"%s\"\n", data->output);
			printf("Return %d Expected: %d\n", ret, (int)strlen(data->output));
		}
		return 1;
	}
	return 0;
}


int main(void)
{
	printf("STRFTIME TEST STARTED\n");
	save_env();

	int i, failed = 0;
	for (i = 0; i < T1_LEN; i++) {
		failed += strftime_assert(&test_vector[i]);
	}
	printf("Performed %d testcases %d failed\n", T1_LEN, failed);
	return 0;
}

