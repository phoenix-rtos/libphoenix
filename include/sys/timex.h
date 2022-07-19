/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/timex.h
 *
 * Copyright 2019 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TIMEX_H_
#define _SYS_TIMEX_H_

#include <sys/time.h>

#define ADJ_OFFSET		0x1
#define ADJ_STATUS		0x2
#define ADJ_TIMECONST	0x4

#define STA_PLL			0x1
#define STA_INS			0x2
#define STA_DEL			0x4


#ifdef __cplusplus
extern "C" {
#endif


struct timex {
	int modes;
	long offset;
	long freq;
	long maxerror;
	long esterror;
	int status;
	long constant;
	long precision;
	long tolerance;
	struct timeval time;
	long tick;
	long ppsfreq;
	long jitter;
	int shift;
	long stabil;
	long jitcnt;
	long calcnt;
	long errcnt;
	long stbcnt;
	int tai;
};


static int adjtimex(struct timex *buf)
{
	return 0;
};


#ifdef __cplusplus
}
#endif


#endif
