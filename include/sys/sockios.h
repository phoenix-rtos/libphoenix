/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sockios.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_SOCKIOS_H_
#define _LIBPHOENIX_SYS_SOCKIOS_H_

#include <net/if6.h>
#include <phoenix/posix/sockios.h>


/* IPv6 ioctls */
#define SIOCGIFDSTADDR_IN6   _IOWR('S', 0x22, struct in6_ifreq)
#define SIOCGIFNETMASK_IN6   _IOWR('S', 0x25, struct in6_ifreq)
#define SIOCDIFADDR_IN6      _IOW('S', 0x19, struct in6_ifreq)
#define SIOCAIFADDR_IN6      _IOW('S', 0x1a, struct in6_aliasreq)
#define SIOCGIFAFLAG_IN6     _IOWR('S', 0x49, struct in6_ifreq)
#define SIOCGIFALIFETIME_IN6 _IOWR('S', 0x51, struct in6_ifreq)


#endif
