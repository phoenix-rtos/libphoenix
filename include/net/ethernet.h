/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/ethernet.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef LIBPHOENIX_NET_ETHERNET_H
#define LIBPHOENIX_NET_ETHERNET_H

#include <stdint.h>

/* IEEE 802.3 Ethernet magic constants. The frame sizes omit the preamble and FCS/CRC (frame check sequence). */
#define ETH_ALEN			6			/* Octets in one ethernet addr */
#define ETH_HLEN			14			/* Total octets in header */
#define ETH_ZLEN			60			/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN		1500		/* Max. octets in payload */
#define ETH_FRAME_LEN		1514		/* Max. octets in frame sans FCS */
#define ETH_FCS_LEN			4			/* Octets in the FCS */

/* Ethernet protocol ID's */
#define	ETHERTYPE_PUP		0x0200          /* Xerox PUP */
#define ETHERTYPE_SPRITE	0x0500		/* Sprite */
#define	ETHERTYPE_IP		0x0800		/* IP */
#define	ETHERTYPE_ARP		0x0806		/* Address resolution */
#define	ETHERTYPE_REVARP	0x8035		/* Reverse ARP */
#define ETHERTYPE_AT		0x809B		/* AppleTalk protocol */
#define ETHERTYPE_AARP		0x80F3		/* AppleTalk ARP */
#define	ETHERTYPE_VLAN		0x8100		/* IEEE 802.1Q VLAN tagging */
#define ETHERTYPE_IPX		0x8137		/* IPX */
#define	ETHERTYPE_IPV6		0x86dd		/* IP protocol version 6 */
#define ETHERTYPE_LOOPBACK	0x9000		/* used to test interfaces */

#define	ETHER_ADDR_LEN	ETH_ALEN                 /* size of ethernet addr */
#define	ETHER_TYPE_LEN	2                        /* bytes in type field */
#define	ETHER_CRC_LEN	4                        /* bytes in CRC field */
#define	ETHER_HDR_LEN	ETH_HLEN                 /* total octets in header */
#define	ETHER_MIN_LEN	(ETH_ZLEN + ETHER_CRC_LEN) /* min packet length */
#define	ETHER_MAX_LEN	(ETH_FRAME_LEN + ETHER_CRC_LEN) /* max packet length */

/* This is a name for the 48 bit ethernet address available on many
   systems.  */
struct ether_addr
{
	uint8_t ether_addr_octet[ETH_ALEN];
} __attribute__ ((__packed__));

/* Ethernet frame header. */
struct ether_header
{
	uint8_t  ether_dhost[ETH_ALEN];	/* destination eth addr	*/
	uint8_t  ether_shost[ETH_ALEN];	/* source ether addr	*/
	uint16_t ether_type;		        /* packet type ID field	*/
} __attribute__ ((__packed__));

#endif
