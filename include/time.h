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


#define SECS_TO_USECS_T(secs) (1000000ULL * (secs))
#define MSECS_TO_USECS_T(msecs) (1000ULL * (msecs))

#include <sys/types.h>


#define CLOCKS_PER_SEC 1000000


#define CLOCK_MONOTONIC     0
#define CLOCK_MONOTONIC_RAW 1
#define CLOCK_REALTIME      2


#ifdef __cplusplus
extern "C" {
#endif


struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};


struct timespec {
	time_t tv_sec;
	long tv_nsec;
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


extern size_t strftime(char *__restrict s, size_t maxsize, const char *__restrict format, const struct tm *__restrict timeptr);


extern clock_t clock(void);


extern char *strptime(const char *__restrict buf, const char *__restrict format, struct tm *__restrict tm);


extern int nanosleep(const struct timespec *req, struct timespec *rem);


#ifdef __cplusplus
}
#endif


#endif
