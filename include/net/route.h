/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/route.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NET_ROUTE_H_
#define _LIBPHOENIX_NET_ROUTE_H_

#include <phoenix/posix/sockrt.h>


#define RTF_UP        0x0001  /* Route usable */
#define RTF_GATEWAY   0x0002  /* Destination is a gateway */
#define RTF_HOST      0x0004  /* Host entry (net otherwise) */
#define RTF_REINSTATE 0x0008  /* Reinstate route after timeout */
#define RTF_DYNAMIC   0x0010  /* Created dynamic (by redirect) */
#define RTF_MODIFIED  0x0020  /* Modified dynamic (by redirect) */
#define RTF_MTU       0x0040  /* Specific MTU for this route */
#define RTF_WINDOW    0x0080  /* Per route window clamping */
#define RTF_IRTT      0x0100  /* Initial round trip time */
#define RTF_REJECT    0x0200  /* Reject route */
#define RTF_MSS       RTF_MTU /* Compatibility alias */


/* For IPv6 */
#define RTF_NONEXTHOP 0x00200000 /* Route with no nexthop */


#endif
