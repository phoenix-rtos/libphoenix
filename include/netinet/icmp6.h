/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netinet/icmp6.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Mateusz Kobak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _NETINET_ICMP6_H
#define _NETINET_ICMP6_H

#include <stdint.h>


struct icmp6_hdr {
	uint8_t icmp6_type;   /* type field */
	uint8_t icmp6_code;   /* code field */
	uint16_t icmp6_cksum; /* checksum field */
	union {
		uint32_t icmp6_un_data32[1]; /* type-specific field */
		uint16_t icmp6_un_data16[2]; /* type-specific field */
		uint8_t icmp6_un_data8[4];   /* type-specific field */
	} icmp6_dataun;
};

#define icmp6_data32   icmp6_dataun.icmp6_un_data32
#define icmp6_data16   icmp6_dataun.icmp6_un_data16
#define icmp6_data8    icmp6_dataun.icmp6_un_data8
#define icmp6_pptr     icmp6_data32[0] /* parameter prob */
#define icmp6_mtu      icmp6_data32[0] /* packet too big */
#define icmp6_id       icmp6_data16[0] /* echo request/reply */
#define icmp6_seq      icmp6_data16[1] /* echo request/reply */
#define icmp6_maxdelay icmp6_data16[0] /* mcast group membership */

#define ICMP6_DST_UNREACH    1
#define ICMP6_PACKET_TOO_BIG 2
#define ICMP6_TIME_EXCEEDED  3
#define ICMP6_PARAM_PROB     4

#define ICMP6_ECHO_REQUEST     128
#define ICMP6_ECHO_REPLY       129
#define MLD_LISTENER_QUERY     130
#define MLD_LISTENER_REPORT    131
#define MLD_LISTENER_REDUCTION 132

#endif /* _NETINET_ICMP6_H */
