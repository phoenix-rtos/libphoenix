/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/proto.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <netdb.h>
#include <string.h>
#include <stdlib.h>


#define PROTOCOLS_NUM (sizeof(protocols) / sizeof(struct protoent_internal))
#define MAX_ALIASES   2


struct protoent_internal {
	const char *name;
	const char *aliases[MAX_ALIASES];
	int proto;
};


static const struct protoent_internal protocols[] = {
	{ "ip", { "IP", NULL }, 0 },
	{ "hopopt", { "HOPOPT", NULL }, 0 },
	{ "icmp", { "ICMP", NULL }, 1 },
	{ "igmp", { "IGMP", NULL }, 2 },
	{ "ggp", { "GGP", NULL }, 3 },
	{ "ipencap", { "IP-ENCAP", NULL }, 4 },
	{ "st", { "ST", NULL }, 5 },
	{ "tcp", { "TCP", NULL }, 6 },
	{ "egp", { "EGP", NULL }, 8 },
	{ "igp", { "IGP", NULL }, 9 },
	{ "pup", { "PUP", NULL }, 12 },
	{ "udp", { "UDP", NULL }, 17 },
	{ "hmp", { "HMP", NULL }, 20 },
	{ "xns-idp", { "XNS-IDP", NULL }, 22 },
	{ "rdp", { "RDP", NULL }, 27 },
	{ "iso-tp4", { "ISO-TP4", NULL }, 29 },
	{ "dccp", { "DCCP", NULL }, 33 },
	{ "xtp", { "XTP", NULL }, 36 },
	{ "ddp", { "DDP", NULL }, 37 },
	{ "idpr-cmtp", { "IDPR-CMTP", NULL }, 38 },
	{ "ipv6", { "IPv6", NULL }, 41 },
	{ "ipv6-route", { "IPv6-Route", NULL }, 43 },
	{ "ipv6-frag", { "IPv6-Frag", NULL }, 44 },
	{ "idrp", { "IDRP", NULL }, 45 },
	{ "rsvp", { "RSVP", NULL }, 46 },
	{ "gre", { "GRE", NULL }, 47 },
	{ "esp", { "IPSEC-ESP", NULL }, 50 },
	{ "ah", { "IPSEC-AH", NULL }, 51 },
	{ "skip", { "SKIP", NULL }, 57 },
	{ "ipv6-icmp", { "IPv6-ICMP", NULL }, 58 },
	{ "ipv6-nonxt", { "IPv6-NoNxt", NULL }, 59 },
	{ "ipv6-opts", { "IPv6-Opts", NULL }, 60 },
	{ "rspf", { "RSPF", "CPHB" }, 73 },
	{ "vmtp", { "VMTP", NULL }, 81 },
	{ "eigrp", { "EIGRP", NULL }, 88 },
	{ "ospf", { "OSPFIGP", NULL }, 89 },
	{ "ax.25", { "AX.25", NULL }, 93 },
	{ "ipip", { "IPIP", NULL }, 94 },
	{ "etherip", { "ETHERIP", NULL }, 97 },
	{ "encap", { "ENCAP", NULL }, 98 },
	{ "pim", { "PIM", NULL }, 103 },
	{ "ipcomp", { "IPCOMP", NULL }, 108 },
	{ "vrrp", { "VRRP", NULL }, 112 },
	{ "l2tp", { "L2TP", NULL }, 115 },
	{ "isis", { "ISIS", NULL }, 124 },
	{ "sctp", { "SCTP", NULL }, 132 },
	{ "fc", { "FC", NULL }, 133 },
	{ "mobility-header", { "Mobility-Header", NULL }, 135 },
	{ "udplite", { "UDPLite", NULL }, 136 },
	{ "mpls-in-ip", { "MPLS-in-IP", NULL }, 137 },
	{ "manet", { NULL, NULL }, 138 },
	{ "hip", { "HIP", NULL }, 139 },
	{ "shim6", { "Shim6", NULL }, 140 },
	{ "wesp", { "WESP", NULL }, 141 },
	{ "rohc", { "ROHC", NULL }, 142 }
};


static char *aliases_common[MAX_ALIASES + 1];
static struct protoent proto_common = { NULL, aliases_common, -1 };


static void protoent_free(struct protoent *proto)
{
	unsigned int i;

	if (proto->p_name) {
		free(proto->p_name);
		proto->p_name = NULL;
	}

	if (proto->p_aliases) {
		for (i = 0; i < MAX_ALIASES && proto->p_aliases[i]; ++i) {
			free(proto->p_aliases[i]);
			proto->p_aliases[i] = NULL;
		}
	}
}


static void protoent_copy(struct protoent *proto, unsigned int num)
{
	unsigned int i;

	proto->p_name = strdup(protocols[num].name);

	for (i = 0; i < MAX_ALIASES && protocols[num].aliases[i]; ++i) {
		proto->p_aliases[i] = strdup(protocols[num].aliases[i]);
	}
	proto->p_aliases[i] = NULL;

	proto->p_proto = protocols[num].proto;
}


struct protoent *getprotobyname(const char *name)
{
	unsigned int i;

	for (i = 0; i < PROTOCOLS_NUM; ++i) {
		if (strcmp(name, protocols[i].name) == 0) {
			protoent_free(&proto_common);
			protoent_copy(&proto_common, i);
			return &proto_common;
		}
	}

	return NULL;
}


struct protoent *getprotobynumber(int proto)
{
	unsigned int i;

	for (i = 0; i < PROTOCOLS_NUM; ++i) {
		if (proto == protocols[i].proto) {
			protoent_free(&proto_common);
			protoent_copy(&proto_common, i);
			return &proto_common;
		}
	}

	return NULL;
}
