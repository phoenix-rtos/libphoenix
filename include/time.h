/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time.h
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_TIME_H_
#define _LIBPHOENIX_TIME_H_

#include <signal.h>
#include <sys/types.h>
#include <phoenix/posix/timespec.h>


#define CLOCKS_PER_SEC 1000000

#define SECS_TO_USECS_T(secs)   (1000000ULL * (secs))
#define MSECS_TO_USECS_T(msecs) (1000ULL * (msecs))


enum { CLOCK_MONOTONIC = 0, CLOCK_MONOTONIC_RAW, CLOCK_REALTIME };


struct tm {
	int tm_sec;   /* Seconds [0, 60] */
	int tm_min;   /* Minutes [0, 59] */
	int tm_hour;  /* Hours [0, 23] */
	int tm_mday;  /* Day of month [1, 31] */
	int tm_mon;   /* Month of year [0, 11] */
	int tm_year;  /* Years since 1900 */
	int tm_wday;  /* Day of week [0, 6] (Sunday = 0) */
	int tm_yday;  /* Day of year [0, 365] */
	int tm_isdst; /* Daylight savings flag */
};


extern char *tzname[2];


extern long timezone;


extern int daylight;


extern void tzset(void);


extern char *asctime(const struct tm *tp);


extern char *asctime_r(const struct tm *tp, char *buf);


extern char *ctime(const time_t *timep);


extern char *ctime_r(const time_t *timep, char *buf);


extern double difftime(time_t t1, time_t t2);


extern struct tm *gmtime(const time_t *timep);


extern struct tm *gmtime_r(const time_t *timep, struct tm *res);


extern struct tm *localtime(const time_t *timep);


extern struct tm *localtime_r(const time_t *timep, struct tm *res);


extern time_t mktime(struct tm *tp);


extern time_t timelocal(struct tm *tm);


extern time_t timegm(struct tm *tm);


extern int clock_gettime(clockid_t clk_id, struct timespec *tp);


extern time_t time(time_t *tp);


extern size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const struct tm *restrict timeptr);


extern clock_t clock(void);


extern char *strptime(const char *restrict buf, const char *restrict format, struct tm *restrict tm);


extern int nanosleep(const struct timespec *req, struct timespec *rem);


#endif
