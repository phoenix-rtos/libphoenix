/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sockios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef LIBPHOENIX_SYS_SOCKIOS_H
#define LIBPHOENIX_SYS_SOCKIOS_H

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>

#define SOCK_IOC_TYPE	'S'

/* Socket configuration controls. */
#define SIOCGIFNAME			_IOWR(SOCK_IOC_TYPE, 0x10, struct ifreq)	/* get name of interface with given index */
#define SIOCGIFCONF			_IOWR(SOCK_IOC_TYPE, 0x12, struct ifconf)   /* get iface list */
#define SIOCGIFFLAGS		_IOWR(SOCK_IOC_TYPE, 0x13, struct ifreq)	/* get interface flags */
#define SIOCSIFFLAGS		_IOW( SOCK_IOC_TYPE, 0x14, struct ifreq)	/* Set interface flags */
#define SIOCGIFADDR			_IOWR(SOCK_IOC_TYPE, 0x15, struct ifreq)	/* get device address */
#define SIOCSIFADDR			_IOW( SOCK_IOC_TYPE, 0x16, struct ifreq)	/* Set device address */
#define SIOCGIFDSTADDR		_IOWR(SOCK_IOC_TYPE, 0x17, struct ifreq)	/* get remote address (point-to-point interfaces) */
#define SIOCSIFDSTADDR		_IOW( SOCK_IOC_TYPE, 0x18, struct ifreq)	/* set remote address (point-to-point interfaces) */
#define SIOCGIFBRDADDR		_IOWR(SOCK_IOC_TYPE, 0x19, struct ifreq)	/* get broadcast address */
#define SIOCSIFBRDADDR		_IOW( SOCK_IOC_TYPE, 0x1a, struct ifreq)	/* set broadcast address */
#define SIOCGIFNETMASK		_IOWR(SOCK_IOC_TYPE, 0x1b, struct ifreq)	/* get network mask */
#define SIOCSIFNETMASK		_IOW( SOCK_IOC_TYPE, 0x1c, struct ifreq)	/* set network mask */
#define SIOCGIFMETRIC		_IOWR(SOCK_IOC_TYPE, 0x1d, struct ifreq)	/* get metric */
#define SIOCSIFMETRIC		_IOW( SOCK_IOC_TYPE, 0x1e, struct ifreq)	/* set metric */
#define SIOCGIFMTU			_IOWR(SOCK_IOC_TYPE, 0x21, struct ifreq)	/* get MTU size */
#define SIOCSIFMTU			_IOW( SOCK_IOC_TYPE, 0x22, struct ifreq)	/* set MTU size */
#define SIOCSIFHWADDR		_IOW( SOCK_IOC_TYPE, 0x24, struct ifreq)	/* set interface MAC address */
#define SIOCGIFHWADDR		_IOWR(SOCK_IOC_TYPE, 0x27, struct ifreq)	/* get interface MAC address */
#define SIOCADDMULTI		_IOWR(SOCK_IOC_TYPE, 0x31, struct ifreq)	/* add multicast address */
#define SIOCDELMULTI		_IOWR(SOCK_IOC_TYPE, 0x32, struct ifreq)	/* remove multicast address */
#define SIOCGIFINDEX		_IOWR(SOCK_IOC_TYPE, 0x33, struct ifreq)	/* get index of interface with given name */

/* unused but needed by busybox ifconfig */
#define SIOCGIFTXQLEN		_IOWR(SOCK_IOC_TYPE, 0x42, struct ifreq)	/* get the tx queue length */
#define SIOCSIFTXQLEN		_IOWR(SOCK_IOC_TYPE, 0x43, struct ifreq)	/* set the tx queue length */

/* Routing table calls.  */
#define SIOCADDRT			_IOW(SOCK_IOC_TYPE, 0x44, struct rtentry)	/* add routing table entry */
#define SIOCDELRT			_IOW(SOCK_IOC_TYPE, 0x45, struct rtentry)	/* delete routing table entry */

#endif // LIBPHOENIX_SYS_SOCKIOS_H
