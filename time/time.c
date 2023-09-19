/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time
 *
 * Copyright 2017, 2023 Phoenix Systems
 * Author: Andrzej Asztemborski, Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>


char *tzname[2];


long timezone;


int daylight;


static const char wdayasc[8][10] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "???" };


static const char monasc[13][10] = { "January", "February", "March", "April", "May",
	"June", "July", "August", "September", "October", "November", "December", "???" };


static int daysofmonth(int month, int leap)
{
	static const int lut[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (month == 1 && leap)
		return 29;
	else
		return lut[month];
}


static inline int isleap(int year)
{
	return !(year % 400) || (!(year % 4) && (year % 100));
}


void tzset(void)
{
	static char tznamestore[2][4];

	/* TODO - env parsing */

	strcpy(tznamestore[0], "UTC");
	tznamestore[1][0] = '\0';
	tzname[0] = tznamestore[0];
	tzname[1] = tznamestore[1];
	timezone = 0;
	daylight = 0;
}


time_t time(time_t *tp)
{
	time_t now, offs;

	gettime(&now, &offs);

	now += offs;

	/* microseconds to seconds */
	now /= 1000 * 1000;

	if (tp != NULL)
		*tp = now;

	return now;
}


int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	time_t now, offs;

	if (tp == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (clk_id != CLOCK_REALTIME && clk_id != CLOCK_MONOTONIC && clk_id != CLOCK_MONOTONIC_RAW) {
		errno = EINVAL;
		return -1;
	}

	gettime(&now, &offs);

	if (clk_id == CLOCK_REALTIME)
		now += offs;

	tp->tv_sec = now / (1000 * 1000);
	now -= tp->tv_sec * 1000 * 1000;
	tp->tv_nsec = now * 1000;

	return EOK;
}


char *asctime_r(const struct tm *tp, char *buf)
{
	int wday, mon;

	wday = tp->tm_wday < 0 || tp->tm_wday > 6 ? 7 : tp->tm_wday;
	mon = tp->tm_mon < 0 || tp->tm_mon > 11 ? 12 : tp->tm_mon;

	sprintf(buf, "%.3s %.3s %d %02d:%02d:%02d %d\n", wdayasc[wday], monasc[mon],
		tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec, tp->tm_year + 1900);

	return buf;
}


char *asctime(const struct tm *tp)
{
	static char buff[32];

	return asctime_r(tp, buff);
}


double difftime(time_t t1, time_t t2)
{
	return (double)t1 - (double)t2;
}


static int epochDaysToYears(int days, int *yearDay)
{
	/* Move day 0 from 1970-01-01 to 2000-02-29. 2000-02-29 closest date when all periods restart.
	 * Algorithm also works correctly when days < 0.
	 */
	int year = 2000, leapDay, fullPeriods;
	days -= 11016;

	static const struct {
		int nDays;         /* Number of days this period has */
		int nYears;        /* Number of years this period has */
		int isZeroDayLeap; /* Is day 0 of this period a leap day (unless a larger period was tried before) */
	} periods[4] = {
		{ 146097, 400, 1 },
		{ 36524, 100, 0 },
		{ 1461, 4, 1 },
		{ 365, 1, 0 },
	};

	for (int i = 0; i < sizeof(periods) / sizeof(*periods); i++) {
		fullPeriods = days / periods[i].nDays;
		days = days % periods[i].nDays;
		if (days < 0) {
			days += periods[i].nDays;
			fullPeriods -= 1;
		}

		year += fullPeriods * periods[i].nYears;
		leapDay = periods[i].isZeroDayLeap;
		if (days == 0) {
			break;
		}
	}

	/* Day 0 is now Feb 29th if leapDay == 1, otherwise it's Feb 28th (leap years have two 0-days!)
	 * Day 1 is always March 1st, but which year day depends on if the year is a leap year.
	 * Day 307 and higher are next year, but before Feb 28th (before possible leap day, so leap year or not doesn't matter)
	 */
	if (days >= 307) {
		year += 1;
		*yearDay = days - 307;
	}
	else if (days == 0) {
		*yearDay = 58 + leapDay;
	}
	else {
		*yearDay = days + (isleap(year) ? 59 : 58);
	}

	return year;
}


struct tm *gmtime_r(const time_t *timep, struct tm *res)
{
	/* We need to be able to represent days as int */
	if ((sizeof(time_t) > sizeof(int)) && (*timep > ((time_t)INT_MAX * (24 * 60 * 60)))) {
		errno = EOVERFLOW;
		return NULL;
	}

	int seconds;
	int days, month, year, isYearLeap;

	days = *timep / (24 * 60 * 60);
	seconds = *timep - (time_t)days * (24 * 60 * 60);
	if (seconds < 0) {
		days--;
		seconds += (24 * 60 * 60);
	}

	res->tm_hour = seconds / (60 * 60);
	seconds = seconds % (60 * 60);

	res->tm_min = seconds / 60;
	res->tm_sec = seconds % 60;

	res->tm_wday = (days + 4) % 7;
	if (res->tm_wday < 0) {
		res->tm_wday += 7;
	}

	year = epochDaysToYears(days, &days);
	isYearLeap = isleap(year);
	res->tm_year = year - 1900;
	res->tm_yday = days;

	for (month = 0; month < 12; month++) {
		int monthLength = daysofmonth(month, isYearLeap);
		if (days >= monthLength) {
			days -= monthLength;
		}
		else {
			break;
		}
	}

	res->tm_mon = month;
	res->tm_mday = days + 1;
	res->tm_isdst = 0;

	return res;
}


struct tm *gmtime(const time_t *timep)
{
	static struct tm tmp;

	return gmtime_r(timep, &tmp);
}


struct tm *localtime_r(const time_t *timep, struct tm *res)
{
	/* TODO - use timezone information */
	return gmtime_r(timep, res);
}


struct tm *localtime(const time_t *timep)
{
	static struct tm tmp;

	return localtime_r(timep, &tmp);
}


char *ctime_r(const time_t *timep, char *buf)
{
	struct tm t, *tp;

	tp = localtime_r(timep, &t);

	return (tp == NULL) ? NULL : asctime_r(&t, buf);
}


char *ctime(const time_t *timep)
{
	static char buff[32];

	return ctime_r(timep, buff);
}

/* Calculate number of leap days between 1970-01-01 and year-01-01 */
static int leapcount(int year)
{
	/* Center on 2000-01-01 for the calculations. There are 8 leap days between 1970-01-01 and that date.
	 * Also subtract 1 because while 2000 is a leap year, its leap day isn't counted yet at 2000-01-01.
	 */
	int leap_days = 8;
	year -= 2001;
	if (year < 0) {
		/* If year is negative, push it into the positive and compensate by subtracting the appropriate
		 * number of leap days from the result. This avoids dealing with the C division operator on negative numbers.
		 */
		int periods = (year - 399) / 400;
		year -= periods * 400;
		leap_days += periods * 97;
	}

	leap_days += year / 400;
	leap_days -= year / 100;
	leap_days += year / 4;

	return leap_days;
}


static time_t _mktimeSkel(const struct tm *tp)
{
	int year = tp->tm_year - 70, leap, i, month;
	time_t res, days;

	year += tp->tm_mon / 12;
	month = tp->tm_mon % 12;
	if (month < 0) {
		year -= 1;
		month += 12;
	}

	days = (time_t)year * 365 + leapcount(year + 1970);
	leap = isleap(year + 1970);
	for (i = 0; i < month; ++i) {
		days += daysofmonth(i, leap);
	}

	days += tp->tm_mday - 1;
	res = days * 24 * 60 * 60;
	res += (tp->tm_hour * 60 + tp->tm_min) * 60;
	res += tp->tm_sec;

	return res;
}


time_t mktime(struct tm *tp)
{
	time_t res;

	tzset();

	res = _mktimeSkel(tp) + timezone - ((daylight && tp->tm_isdst > 0) ? 3600 : 0);
	if (localtime_r(&res, tp) == NULL) {
		return -1;
	}

	return res;
}


time_t timelocal(struct tm *tm)
{
	return mktime(tm);
}


time_t timegm(struct tm *tm)
{
	time_t res;

	res = _mktimeSkel(tm);
	if (gmtime_r(&res, tm) == NULL) {
		return -1;
	}

	return res;
}


size_t strftime(char *__restrict s, size_t maxsize, const char *__restrict format, const struct tm *__restrict timeptr)
{
	int res;
	size_t size = 0;
	const char *c = format, *tmp;
	struct tm time;

	while (size < maxsize) {
		if (*c == '%') {
			c++;
			switch (*c) {
			case 'A':
				tmp = wdayasc[timeptr->tm_wday < 7 ? timeptr->tm_wday : 7];
				res = snprintf(s + size, maxsize - size, "%s", tmp);
				break;
			case 'a':
				tmp = wdayasc[timeptr->tm_wday < 7 ? timeptr->tm_wday : 7];
				res = snprintf(s + size, maxsize - size, "%.3s", tmp);
				break;
			case 'B':
				tmp = monasc[timeptr->tm_mon < 12 ? timeptr->tm_mon : 12];
				res = snprintf(s + size, maxsize - size, "%s", tmp);
				break;
			case 'b':
				tmp = monasc[timeptr->tm_mon < 12 ? timeptr->tm_mon : 12];
				res = snprintf(s + size, maxsize - size, "%.3s", tmp);
				break;
			case 'c':
				res = snprintf(s + size, maxsize - size, "%.3s %.3s %u %02u:%02u:%02u %u",
						wdayasc[timeptr->tm_wday < 7 ? timeptr->tm_wday : 7],
						monasc[timeptr->tm_mon < 12 ? timeptr->tm_mon : 12],
						timeptr->tm_mday,
						timeptr->tm_hour,
						timeptr->tm_min,
						timeptr->tm_sec,
						1900 + timeptr->tm_year);
				break;
			case 'd':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_mday);
				break;
			case 'e':
				res = snprintf(s + size, maxsize - size, "%2u", timeptr->tm_mday);
				break;
			case 'H':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_hour);
				break;
			case 'j':
				res = snprintf(s + size, maxsize - size, "%03u", timeptr->tm_yday + 1);
				break;
			case 'M':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_min);
				break;
			case 'm':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_mon < 12 ? timeptr->tm_mon + 1 : 13);
				break;
			case 'S':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_sec);
				break;
			case 's':
				memcpy(&time, timeptr, sizeof(struct tm));
				res = snprintf(s + size, maxsize - size, "%llu", mktime(&time));
				break;
			case 'w':
				res = snprintf(s + size, maxsize - size, "%u", timeptr->tm_wday < 7 ? timeptr->tm_wday : 7);
				break;
			case 'Y':
				res = snprintf(s + size, maxsize - size, "%u", 1900 + timeptr->tm_year);
				break;
			case 'y':
				res = snprintf(s + size, maxsize - size, "%02u", timeptr->tm_year % 100);
				break;
			case 'Z':
				c++;
				continue;
			case '%':
				s[size] = *c;
				res = 1;
				break;
			default: /* Unsupported conversion specifier */
				if (maxsize - size < 3)
					return 0;
				s[size++] = '%';
				s[size++] = *(c++);
				continue;
			}

			if (res >= maxsize - size)
				return 0;
			size += res;

		} else if (*c == 0) {
			s[size] = 0;
			return size;
		} else {
			s[size++] = *c;
		}
		c++;
	}

	/* Length of the result string would exceed max size */
	return 0;
}


clock_t clock(void)
{
	return (clock_t)-1;
}

char *strptime(const char *__restrict buf, const char *__restrict format, struct tm *__restrict tm)
{
	return NULL;
}


extern int nsleep(time_t *sec, long *nsec);


int nanosleep(const struct timespec *req, struct timespec *rem)
{
	time_t sec = req->tv_sec;
	long nsec = req->tv_nsec;
	int ret;

	ret = nsleep(&sec, &nsec);

	if (ret == -EINTR && rem != NULL) {
		rem->tv_sec = sec;
		rem->tv_nsec = nsec;
	}

	return SET_ERRNO(ret);
}
