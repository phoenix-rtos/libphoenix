/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * POSIX-compatibility module, syslog interface
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#ifndef PATH_LOG
#define PATH_LOG "/dev/log"
#endif

#ifndef MAX_LOG_SIZE
#define MAX_LOG_SIZE 512
#endif

static struct {
	int open;
	int logfd;
	int logmask;
	int logopt;
	int facility;

	int offset;
	char buf[MAX_LOG_SIZE];
} syslog_common;


void closelog(void)
{
	if (syslog_common.open)
		close(syslog_common.logfd);

	syslog_common.open = 0;
}


void openlog(const char *ident, int logopt, int facility)
{
	if (ident != NULL) {
		strncpy(syslog_common.buf, ident, MAX_LOG_SIZE);
		syslog_common.offset = strlen(ident);
	}

	if (logopt & LOG_NDELAY) {
		if (!syslog_common.open) {
			syslog_common.logfd = open(PATH_LOG, O_RDONLY | O_NONBLOCK);
			syslog_common.open = syslog_common.logfd != -1;
		}
	}

	syslog_common.logopt = logopt;
	syslog_common.facility = facility;
}


int setlogmask(int maskpri)
{
	int prevmask = syslog_common.logmask;

	if (maskpri)
		syslog_common.logmask = ~maskpri;

	return ~prevmask;
}


void vsyslog(int priority, const char *format, va_list ap)
{
	int cnt;

	if ((1 << priority) & syslog_common.logmask)
		return;

	if (!syslog_common.open) {
		if ((syslog_common.logfd = open(PATH_LOG, O_RDONLY | O_NONBLOCK)) == -1)
			return;

		syslog_common.open = 1;
	}

	if ((cnt = vsnprintf(syslog_common.buf + syslog_common.offset, MAX_LOG_SIZE - syslog_common.offset, format, ap)) > 0)
		write(syslog_common.logfd, syslog_common.buf, syslog_common.offset + cnt);
}


void syslog(int priority, const char *message, ... /* arguments */)
{
	va_list ap;

	va_start(ap, message);
	vsyslog(priority, message, ap);
	va_end(ap);
}
