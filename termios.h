/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_TERMIOS_H_
#define _LIBPHOENIX_TERMIOS_H_

typedef int speed_t;
typedef int tcflag_t;
typedef int cc_t;


enum {
	VEOF = 0, VEOL, VERASE, VINTR, VKILL, VMIN, VQUIT, VSTART, VSTOP, VSUSP, VTIME, NCCS
};


struct termios {
	tcflag_t  c_iflag;
	tcflag_t  c_oflag;
	tcflag_t  c_cflag;
	tcflag_t  c_lflag;
	cc_t      c_cc[NCCS];
};


/* c_iflag */
enum {
	BRKINT = 1 << 0,
	ICRNL = 1 << 0,
	IGNBRK = 1 << 0,
	IGNCR = 1 << 0,
	IGNPAR = 1 << 0,
	INLCR = 1 << 0,
	INPCK = 1 << 0,
	ISTRIP = 1 << 0,
	IUCLC = 1 << 0,
	IXANY = 1 << 0,
	IXOFF = 1 << 0,
	IXON = 1 << 0,
	IMAXBEL = 1 << 0,
	PARMRK = 1 << 0,
};

/* c_oflag */
enum {
	OPOST = 1 << 0,
	OLCUC = 1 << 0,
	ONLCR = 1 << 0,
	OCRNL = 1 << 0,
	ONOCR = 1 << 0,
	ONLRET = 1 << 0,
	OFILL = 1 << 0,
	NLDLY = 1 << 0,
	NL0 = 1 << 0,
	NL1 = 1 << 0,
	CRDLY = 1 << 0,
	CR0 = 1 << 0,
	CR1 = 1 << 0,
	CR2 = 1 << 0,
	CR3 = 1 << 0,
	TABDLY = 1 << 0,
	TAB0 = 1 << 0,
	TAB1 = 1 << 0,
	TAB2 = 1 << 0,
	TAB3 = 1 << 0,
	BSDLY = 1 << 0,
	BS0 = 1 << 0,
	BS1 = 1 << 0,
	VTDLY = 1 << 0,
	VT0 = 1 << 0,
	VT1 = 1 << 0,
	FFDLY = 1 << 0,
	FF0 = 1 << 0,
	FF1 = 1 << 0,
};

/* baud rate */
enum {
	B0 = 0,
	B50,
	B75,
	B110,
	B134,
	B150,
	B200,
	B300,
	B600,
	B1200,
	B1800,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
};


/* c_cflag */
enum {
	CS5 = 0, CS6, CS7, CS8,
	CSIZE = 3,

	CSTOPB = 1 << 3,
	CREAD  = 1 << 4,
	PARENB = 1 << 5,
	PARODD = 1 << 6,
	HUPCL  = 1 << 7,
	CLOCAL = 1 << 8,
};


/* c_lflag */
enum {
	ECHO   = 1 << 0,
	ECHOE  = 1 << 1,
	ECHOK  = 1 << 2,
	ECHONL = 1 << 3,
	ICANON = 1 << 4,
	IEXTEN = 1 << 5,
	ISIG   = 1 << 6,
	NOFLSH = 1 << 7,
	TOSTOP = 1 << 8,
	XCASE  = 1 << 9,
};


enum { TCIFLUSH, TCOFLUSH, TCIOFLUSH };

enum { TCSANOW, TCSADRAIN, TCSAFLUSH };

enum { TCIOFF, TCION, TCOOFF, TCOON };


int tcgetattr(int fildes, struct termios *termios_p);


int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p);


#endif
