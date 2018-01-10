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


struct winsize
{
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};


enum { TIOCGWINSZ };


int ioctl(int fd, int cmd, ... );


#endif
