/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syslog/syslog.c
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Jan Sikorski, Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/minmax.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>


#ifndef PATH_LOG
#define PATH_LOG "/dev/log"
#endif

#ifndef MAX_LOG_SIZE
#define MAX_LOG_SIZE 256
#endif

static struct {
	const char *ident;

	int open;
	int logfd;
	int logmask;
	int logopt;
	int facility;
	int configured;

	struct sockaddr_un addr;
	socklen_t addrlen;

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
	}
	else {
		syslog_common.ident = argv_progname;
	}

	if (logopt & LOG_NDELAY) {
		if (!syslog_common.open) {
			syslog_common.addr.sun_family = AF_UNIX;
			strncpy(syslog_common.addr.sun_path, PATH_LOG, sizeof(syslog_common.addr.sun_path));
			syslog_common.addrlen = SUN_LEN(&syslog_common.addr);
			syslog_common.logfd = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
			syslog_common.open = syslog_common.logfd != -1;
		}
	}

	syslog_common.configured = 1;
	syslog_common.logmask = 0;
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
	int cnt, prefix_size;
	ssize_t err = 0;

	if ((1 << LOG_PRI(priority)) & syslog_common.logmask)
		return;

	if (!syslog_common.open)
		openlog(syslog_common.ident, syslog_common.logopt | LOG_NDELAY, syslog_common.configured ? syslog_common.facility : LOG_USER);

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
		err = sendto(syslog_common.logfd, syslog_common.buf, len + 1, 0, (struct sockaddr *)&syslog_common.addr, syslog_common.addrlen);

	/* output to stderr if logging device is not available */
	if (syslog_common.logopt & LOG_PERROR || !syslog_common.open || err < 0) {
		if (syslog_common.buf[len - 1] != '\n')
			syslog_common.buf[len] = '\n';
		write(STDERR_FILENO, syslog_common.buf + prefix_size, len - prefix_size + 1);
	}
}


void syslog(int priority, const char *message, ... /* arguments */)
{
	va_list ap;

	va_start(ap, message);
	vsyslog(priority, message, ap);
	va_end(ap);
}
