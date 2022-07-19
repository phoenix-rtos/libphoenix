/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * termios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_TERMIOS_H_
#define _LIBPHOENIX_TERMIOS_H_


#include <sys/ioctl.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned cc_t;
typedef int speed_t;
typedef int tcflag_t;


/* c_cc characters */
/* these need to be define's as various programs test them in preprocessor macros */
#define VINTR 		0
#define VQUIT 		1
#define VERASE 		2
#define VKILL 		3
#define VEOF 		4
#define VTIME 		5
#define VMIN 		6
#define VSTART 		7
#define VSTOP 		8
#define VSUSP 		9
#define VEOL 		10
#define VREPRINT 	11
#define VDISCARD 	12
#define VWERASE 	13
#define VLNEXT 		14
#define VERASE2 	15
#define VEOL2 		16

#define NCCS 		17

#define _POSIX_VDISABLE '\0'


struct termios {
	tcflag_t  c_iflag;
	tcflag_t  c_oflag;
	tcflag_t  c_cflag;
	tcflag_t  c_lflag;
	cc_t      c_cc[NCCS];
	speed_t   c_ispeed;
	speed_t   c_ospeed;
};


/* c_iflag */
enum {
	BRKINT = 1 << 0,
	ICRNL = 1 << 1,
	IGNBRK = 1 << 2,
	IGNCR = 1 << 3,
	IGNPAR = 1 << 4,
	INLCR = 1 << 5,
	INPCK = 1 << 6,
	ISTRIP = 1 << 7,
	IUCLC = 1 << 8,
	IXANY = 1 << 9,
	IXOFF = 1 << 10,
	IXON = 1 << 11,
	IMAXBEL = 1 << 12,
	PARMRK = 1 << 13,
};

/* c_oflag */
enum {
	OPOST = 1 << 0,
	OLCUC = 1 << 1,
	ONLCR = 1 << 2,
	OCRNL = 1 << 3,
	ONOCR = 1 << 4,
	ONLRET = 1 << 5,
	OFILL = 1 << 6,
	NLDLY = 1 << 7,
	NL0 = 1 << 8,
	NL1 = 1 << 9,
	CRDLY = 1 << 10,
	CR0 = 1 << 11,
	CR1 = 1 << 12,
	CR2 = 1 << 13,
	CR3 = 1 << 14,
	TABDLY = 1 << 15,
	TAB0 = 1 << 16,
	TAB1 = 1 << 17,
	TAB2 = 1 << 18,
	TAB3 = 1 << 19,
	XTABS = TAB3,
	BSDLY = 1 << 20,
	BS0 = 1 << 21,
	BS1 = 1 << 22,
	VTDLY = 1 << 23,
	VT0 = 1 << 24,
	VT1 = 1 << 25,
	FFDLY = 1 << 26,
	FF0 = 1 << 27,
	FF1 = 1 << 28,
};

/* baud rate */
/* these need to be define's as various programs test them in preprocessor macros */
#define	B0 		0
#define	B50		1
#define	B75		2
#define	B110		3
#define	B134		4
#define	B150		5
#define	B200		6
#define	B300		7
#define	B600		8
#define	B1200		9
#define	B1800		10
#define	B2400		11
#define	B4800		12
#define	B9600		13
#define	B19200		14
#define	B38400		15
#define	B57600		16
#define	B115200		17
#define	B230400		18
#define	B460800		19


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
	ECHO    = 1 << 0,
	ECHOE   = 1 << 1,
	ECHOK   = 1 << 2,
	ECHONL  = 1 << 3,
	ICANON  = 1 << 4,
	IEXTEN  = 1 << 5,
	ISIG    = 1 << 6,
	NOFLSH  = 1 << 7,
	TOSTOP  = 1 << 8,
	XCASE   = 1 << 9,
	ECHOCTL = 1 << 10,
	ECHOKE  = 1 << 11,
};

struct winsize
{
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

/* ioctls */
#define TCGETS      _IOR('t', 0x1, struct termios)
#define TCSETS      _IOW('t', 0x2, struct termios)
#define TCSETSW     _IOW('t', 0x3, struct termios)
#define TCSETSF     _IOW('t', 0x4, struct termios)
#define TCFLSH      _IOV('t', 0x5, int)
#define TCDRAIN     _IO('t', 0x6)
#define TCSBRK      _IOV('t', 0x9, int)
#define TIOCSCTTY   _IOV('t', 0xE, int)
#define TIOCGPGRP   _IOR('t', 0xF, pid_t)
#define TIOCSPGRP   _IOW('t', 0x10, pid_t)
#define TIOCGWINSZ  _IOR('t', 0x13, struct winsize)
#define TIOCSWINSZ  _IOW('t', 0x14, struct winsize)
#define TIOCNOTTY   _IO('t', 0x22)
#define TIOCGSID    _IOR('t', 0x29, pid_t)

/* tcflush */
enum { TCIFLUSH, TCOFLUSH, TCIOFLUSH };

/* tcsetattr */
enum { TCSANOW, TCSADRAIN, TCSAFLUSH };

/* tcflow */
enum { TCIOFF, TCION, TCOOFF, TCOON };


int tcgetattr(int fildes, struct termios *termios_p);


int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p);

int tcsendbreak(int fd, int duration);
int tcflush(int fd, int queue_selector);
int tcdrain(int fd);

int tcsetpgrp(int fd, pid_t pgrp);
pid_t tcgetpgrp(int fd);

pid_t tcgetsid(int fd);


void cfmakeraw(struct termios *termios_p);


static inline speed_t cfgetispeed(const struct termios *termios_p)
{
	return termios_p->c_ispeed;
}

static inline speed_t cfgetospeed(const struct termios *termios_p)
{
	return termios_p->c_ospeed;
}

static inline int cfsetispeed(struct termios *termios_p, speed_t speed)
{
	if (speed == 0) /* needed by POSIX */
		speed = termios_p->c_ospeed;

	termios_p->c_ispeed = speed;
	return 0;
}

static inline int cfsetospeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ospeed = speed;
	return 0;
}

static inline int cfsetspeed(struct termios *termios_p, speed_t speed)
{
	termios_p->c_ospeed = speed;
	termios_p->c_ispeed = speed;
	return 0;
}


#ifdef __cplusplus
}
#endif


#endif
