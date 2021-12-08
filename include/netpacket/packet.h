/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netpacket/packet.h

 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is a part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETPACKET_PACKET_H_
#define _LIBPHOENIX_NETPACKET_PACKET_H_


/* Packet types */
#define PACKET_HOST      0 /* To us */
#define PACKET_BROADCAST 1 /* To all */
#define PACKET_MULTICAST 2 /* To group */
#define PACKET_OTHERHOST 3 /* To someone else */
#define PACKET_OUTGOING  4 /* Originated by us */ 
#define PACKET_LOOPBACK  5 /* MC/BRD frame looped back */


struct sockaddr_ll {
	unsigned short int sll_family;
	unsigned short int sll_protocol;
	int sll_ifindex;
	unsigned short int sll_hatype;
	unsigned char sll_pkttype;
	unsigned char sll_halen;
	unsigned char sll_addr[8];
};


#endif
