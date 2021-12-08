/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Andrzej Asztemborski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


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


static inline int leapcount(int year)
{
	year -= 1;

	if (year < 1972)
		return 0;

	return ((year - 1968) / 4) - ((year - 2000) / 100) + ((year - 2000) / 400);
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
	now += 500 * 1000;
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


struct tm *gmtime_r(const time_t *timep, struct tm *res)
{
	time_t sec = *timep, minute = 0, hou = 0, t;
	int day = 0, leap, leapcnt;

	if ((t = sec % 60) != sec) {
		minute = sec / 60;
		sec = t;
	}

	if ((t = minute % 60) != minute) {
		hou = minute / 60;
		minute = t;
	}

	if ((t = hou % 24) != hou) {
		day = hou / 24;
		hou = t;
	}

	res->tm_sec = sec;
	res->tm_min = minute;
	res->tm_hour = hou;

	res->tm_wday = (day + 4) % 7;

	res->tm_year = 70 + day / 365;
	leapcnt = leapcount(res->tm_year + 1900);
	day -= leapcnt;
	res->tm_year = 70 + day / 365;

	if (leapcnt != leapcount(res->tm_year + 1900)) {
		leapcnt = leapcount(res->tm_year + 1900);
		++day;
	}

	day -= (res->tm_year - 70) * 365;
	leap = isleap(res->tm_year + 1900);
	res->tm_yday = day;

	for (res->tm_mon = 0; day >= 0 && res->tm_mon < 12; ++res->tm_mon)
		day -= daysofmonth(res->tm_mon, leap);

	if (day < 0) {
		--res->tm_mon;
		day += daysofmonth(res->tm_mon, leap);
	}
	else {
		res->tm_mon = 0;
		++res->tm_year;
	}

	res->tm_mday = day + 1;
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
	struct tm t;

	localtime_r(timep, &t);

	return asctime_r(&t, buf);
}


char *ctime(const time_t *timep)
{
	static char buff[32];

	return ctime_r(timep, buff);
}


static time_t _mktimeSkel(struct tm *tp)
{
	int year = tp->tm_year - 70, leap, i;
	time_t res, days;

	year += tp->tm_mon / 12;
	tp->tm_mon %= 12;

	days = year * 365 + tp->tm_mday + leapcount(year + 1970);
	leap = isleap(year + 1970);

	for (i = 0; i < tp->tm_mon; ++i)
		days += daysofmonth(i, leap);

	res = (days - 1) * 24 * 60 * 60;
	res += (tp->tm_hour * 60 + tp->tm_min) * 60;
	res += tp->tm_sec;

	return res;
}


time_t mktime(struct tm *tp)
{
	time_t res;

	tzset();

	res = _mktimeSkel(tp) + timezone - ((daylight && tp->tm_isdst > 0) ? 3600 : 0);
	localtime_r(&res, tp);

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
	gmtime_r(&res, tm);

	return res;
}


size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const struct tm *restrict timeptr)
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


char *strptime(const char *restrict buf, const char *restrict format, struct tm *restrict tm)
{
	return NULL;
}


int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return 0;
}
