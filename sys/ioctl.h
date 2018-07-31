/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_IOCTL_H_
#define _LIBPHOENIX_IOCTL_H_

#include <sys/msg.h>


#define IOCPARM_MASK		0x1fff
#define IOCPARM_LEN(x)		(((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x)		((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x)			(((x) >> 8) & 0xff)


#define IOC_VOID			0x00000000
#define IOC_OUT				0x40000000
#define IOC_IN				0x80000000
#define IOC_INOUT			(IOC_IN | IOC_OUT)
#define IOC_DIRMASK			0xe0000000


#define _IOC(inout,group,num,len)	((unsigned long) (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define _IO(g,n)			_IOC(IOC_VOID, (g), (n), 0)
#define _IOR(g,n,t)			_IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g,n,t)			_IOC(IOC_IN, (g), (n), sizeof(t))
#define _IOWR(g,n,t)		_IOC(IOC_INOUT, (g), (n), sizeof(t))


struct winsize
{
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};


enum { TIOCGWINSZ, TIOCSWINSZ, TIOCSCTTY };


int ioctl(int fd, int cmd, ... );


const void * ioctl_unpack(const msg_t *msg, unsigned long *request);


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data);


#endif
