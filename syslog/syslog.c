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
#define MAX_LOG_SIZE 256
#endif

static struct {
	const char* ident;

	int open;
	int logfd;
	int logmask;
	int logopt;
	int facility;

	char buf[MAX_LOG_SIZE];
} syslog_common;


void closelog(void)
{
	if (syslog_common.open)
		close(syslog_common.logfd);

	syslog_common.open = 0;
}

extern const char *argv_progname;

void openlog(const char *ident, int logopt, int facility)
{
	if (ident != NULL) {
		syslog_common.ident = ident;
	} else {
		syslog_common.ident = argv_progname;
	}

	if (logopt & LOG_NDELAY) {
		if (!syslog_common.open) {
			syslog_common.logfd = open(PATH_LOG, O_RDONLY | O_NONBLOCK);
			syslog_common.open = syslog_common.logfd != -1;
		}
	}

	syslog_common.logmask = 0;
	syslog_common.logopt = logopt;
	syslog_common.facility = (facility > 0) ? facility : LOG_USER;
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
	int cnt, prefix_size;

	if ((1 << LOG_PRI(priority)) & syslog_common.logmask)
		return;

	if (!syslog_common.open)
		openlog(syslog_common.ident, syslog_common.logopt | LOG_NDELAY, syslog_common.facility);

	if (LOG_FAC(priority) == 0)
		priority |= syslog_common.facility;

	if (syslog_common.logopt & LOG_PID)
		prefix_size = snprintf(syslog_common.buf, MAX_LOG_SIZE, "<%d> %s[%d]: ", priority, syslog_common.ident, getpid());
	else
		prefix_size = snprintf(syslog_common.buf, MAX_LOG_SIZE, "<%d> %s: ", priority, syslog_common.ident);

	if ((cnt = vsnprintf(syslog_common.buf + prefix_size, MAX_LOG_SIZE - prefix_size, format, ap)) < 0)
		return;

	size_t len = min(prefix_size + cnt, MAX_LOG_SIZE - 1);
	syslog_common.buf[len] = '\0';

	if (syslog_common.open)
		write(syslog_common.logfd, syslog_common.buf, len);

	/* output to stderr if logging device is not available */
	if (syslog_common.logopt & LOG_PERROR || !syslog_common.open) {
		syslog_common.buf[len] = '\n';
		write(STDERR_FILENO, syslog_common.buf + prefix_size, len - prefix_size);
	}
}


void syslog(int priority, const char *message, ... /* arguments */)
{
	va_list ap;

	va_start(ap, message);
	vsyslog(priority, message, ap);
	va_end(ap);
}
