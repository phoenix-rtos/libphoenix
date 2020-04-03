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
#include "test_common.h"

#define NCOLS 9
#define T1_LEN 50

static const time_t input_vector[T1_LEN];
static const int output_vector[T1_LEN][NCOLS];


/* help function for future test generation if needed - not used in test functions */
void generate_input_host(void)
{
	int i;
	struct tm t;
	srand(time(NULL));
	for (i = 0; i < T1_LEN; i++) {
		t = (struct tm) { .tm_sec = rand() % 59, .tm_min = rand() % 59, .tm_hour = rand() % 23,
						  .tm_mday = rand() % 30, .tm_mon = rand() % 11, .tm_year = 1980 + rand() % 60,
						  .tm_wday = 0, .tm_yday = 0, .tm_isdst = 0 };
		printf("%lld", mktime(&t));
		if (i != T1_LEN - 1)
			printf(",\n");
	}
	printf("\n");
}



/* help function for future test generation if needed - not used in test functions */
void generate_output_host(void)
{
	int i;
	struct tm *t;
	printf("Printing host output data. Struct tm member values:\n");
	for (i = 0; i < T1_LEN; i++) {
		t = gmtime(&input_vector[i]);
		printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d }",
				   t->tm_sec, t->tm_min, t->tm_hour, t->tm_mday, t->tm_mon, t->tm_year,
				   t->tm_wday, t->tm_yday, t->tm_isdst);
		if (i != T1_LEN - 1)
			printf(",\n");
	}
	printf("\n");
}


int gmtime_assert(const time_t input, const int output[])
{
	struct tm *t, t_exp;
	char buff[120];
	init_tm(&t_exp, output);
	t = gmtime(&input);
	if (compare_tm(t, &t_exp)) {
		if (verbose_test()) {
			tm_to_str(t, buff);
			printf("Testcase failed\nTimestamp: %lld\nOutput tm: %s\n", input, buff);
			tm_to_str(&t_exp, buff);
			printf("Expected: %s \n\n", buff);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(void)
{
	printf("GMTIME TEST STARTED\n");
	save_env();

	int i, failed = 0;
	for (i=0; i < T1_LEN; i++) {
		failed += gmtime_assert(input_vector[i], output_vector[i]);
	}
	printf("Performed %d testcases %d failed\n", T1_LEN, failed);
	return 0;
}

static const time_t input_vector[] = {
	/* 29 FEB 2016 - leap year */
	61414866000,
	/* 1 MAR 2016 - leap year */
	61414952400,
	/* 1 MAR 2017 - non-leap year */
	61417630800,
	/* rest are just various dates */
	61908090106,
	61541479484,
	60434228293,
	60825353990,
	61209399605,
	60679184290,
	61234186708,
	60685313162,
	62006258944,
	60331937805,
	61177051253,
	61185743291,
	62088464479,
	60709737607,
	60963440574,
	61198730883,
	60938948756,
	61527462921,
	61316005195,
	60984357045,
	62001279630,
	60849616891,
	61473428358,
	61758255608,
	62001910453,
	61055224338,
	61974639093,
	61546923339,
	60728819626,
	61285432866,
	61607162558,
	60889606233,
	60397042005,
	61545619354,
	61049031759,
	60792060946,
	60857739371,
	60788198100,
	60663232364,
	61114943347,
	61611439606,
	61118695878,
	61740489742,
	61465651360,
	61298446000,
	61327766567,
	61538301611
};

static const int output_vector[][NCOLS] = {
	{ 0, 0, 5, 29, 1, 2016, 2, 59, 0 },
	{ 0, 0, 5, 1, 2, 2016, 3, 60, 0 },
	{ 0, 0, 5, 1, 3, 2016, 6, 91, 0 },
	{ 46, 41, 19, 16, 9, 2031, 5, 288, 0 },
	{ 44, 24, 15, 4, 2, 2020, 4, 63, 0 },
	{ 13, 38, 5, 31, 0, 1985, 6, 30, 0 },
	{ 50, 39, 3, 24, 5, 1997, 4, 174, 0 },
	{ 5, 0, 3, 26, 7, 2009, 4, 237, 0 },
	{ 10, 58, 8, 5, 10, 1992, 6, 309, 0 },
	{ 28, 18, 0, 9, 5, 2010, 4, 159, 0 },
	{ 2, 26, 7, 15, 0, 1993, 0, 14, 0 },
	{ 4, 49, 0, 26, 10, 2034, 1, 329, 0 },
	{ 45, 36, 7, 4, 10, 1981, 5, 307, 0 },
	{ 53, 20, 17, 16, 7, 2008, 0, 228, 0 },
	{ 11, 48, 7, 25, 10, 2008, 3, 329, 0 },
	{ 19, 41, 11, 4, 6, 2037, 0, 184, 0 },
	{ 7, 0, 0, 25, 9, 1993, 3, 297, 0 },
	{ 54, 2, 9, 9, 10, 2001, 6, 312, 0 },
	{ 3, 28, 15, 24, 3, 2009, 6, 113, 0 },
	{ 56, 45, 21, 29, 0, 2001, 2, 28, 0 },
	{ 21, 55, 9, 24, 8, 2019, 3, 266, 0 },
	{ 55, 39, 23, 10, 0, 2013, 5, 9, 0 },
	{ 45, 10, 11, 9, 6, 2002, 3, 189, 0 },
	{ 30, 40, 9, 29, 8, 2034, 6, 271, 0 },
	{ 31, 21, 23, 31, 2, 1998, 4, 89, 0 },
	{ 18, 19, 0, 7, 0, 2018, 1, 6, 0 },
	{ 8, 0, 15, 16, 0, 2027, 0, 15, 0 },
	{ 13, 54, 16, 6, 9, 2034, 6, 278, 0 },
	{ 18, 32, 16, 6, 9, 2004, 4, 279, 0 },
	{ 33, 31, 1, 25, 10, 2033, 6, 328, 0 },
	{ 39, 35, 15, 6, 4, 2020, 4, 126, 0 },
	{ 46, 33, 20, 2, 5, 1994, 6, 152, 0 },
	{ 6, 21, 3, 23, 0, 2012, 2, 22, 0 },
	{ 38, 42, 20, 3, 3, 2022, 1, 92, 0 },
	{ 33, 30, 19, 7, 6, 1999, 5, 187, 0 },
	{ 45, 6, 20, 27, 10, 1983, 2, 330, 0 },
	{ 34, 22, 13, 21, 3, 2020, 3, 111, 0 },
	{ 39, 22, 0, 27, 6, 2004, 3, 208, 0 },
	{ 46, 35, 19, 3, 5, 1996, 3, 154, 0 },
	{ 11, 36, 23, 3, 6, 1998, 0, 183, 0 },
	{ 0, 35, 2, 20, 3, 1996, 1, 110, 0 },
	{ 44, 52, 17, 4, 4, 1992, 3, 124, 0 },
	{ 7, 9, 21, 28, 7, 2006, 2, 239, 0 },
	{ 46, 46, 8, 23, 4, 2022, 2, 142, 0 },
	{ 18, 31, 7, 11, 9, 2006, 4, 283, 0 },
	{ 22, 2, 0, 25, 5, 2026, 5, 175, 0 },
	{ 40, 2, 0, 9, 9, 2017, 2, 281, 0 },
	{ 40, 6, 18, 21, 5, 2012, 5, 172, 0 },
	{ 47, 42, 2, 27, 4, 2013, 2, 146, 0 },
	{ 11, 40, 20, 27, 0, 2020, 2, 26, 0 }
};


