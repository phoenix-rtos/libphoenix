/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/if.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NET_IF_H
#define _LIBPHOENIX_NET_IF_H


#define	IFF_UP				0x1			/* Interface is up */
#define	IFF_BROADCAST		0x2			/* Broadcast address valid */
#define	IFF_DEBUG			0x4			/* Turn on debugging */
#define	IFF_LOOPBACK		0x8			/* Is a loopback net */
#define	IFF_POINTOPOINT		0x10		/* Interface is point-to-point link */
#define	IFF_NOTRAILERS		0x20		/* Avoid use of trailers */
#define	IFF_RUNNING			0x40		/* Resources allocated */
#define	IFF_NOARP			0x80		/* No address resolution protocol */
#define	IFF_PROMISC			0x100		/* Receive all packets */
#define	IFF_ALLMULTI		0x200		/* Receive all multicast packets */
#define	IFF_OACTIVE			0x400		/* Transmission in progress */
#define	IFF_SIMPLEX			0x800		/* Can't hear own transmissions */
#define	IFF_LINK0			0x1000		/* Per link layer defined bit */
#define	IFF_LINK1			0x2000		/* Per link layer defined bit */
#define	IFF_MULTICAST		0x4000		/* Supports multicast */
#define	IFF_MASTER			IFF_LINK0	/* UNUSED but needed by ifconfig */
#define	IFF_SLAVE		    IFF_LINK1	/* UNUSED but needed by ifconfig */
#define	IFF_DYNAMIC			0x8000		/* The addresses are lost when the interface goes down */

#define IFHWADDRLEN 6
#define IFNAMSIZ    16
#define IF_NAMESIZE IFNAMSIZ /* POSIX compliant alias */

struct ifmap {
    unsigned long mem_start;
    unsigned long mem_end;
    unsigned short base_addr;
    unsigned char irq;
    unsigned char dma;
    unsigned char port;
};

/*
 * Interface request structure used for socket
 * ioctl's. All interface ioctl's must have parameter
 * definitions which begin with ifr_name. The
 * remainder may be interface specific.
 */
struct ifreq {
	union {
		char ifrn_name[IFNAMSIZ];
	} ifr_ifrn;

	union {
		struct sockaddr ifru_addr;
		struct sockaddr ifru_dstaddr;
		struct sockaddr ifru_broadaddr;
		struct sockaddr ifru_netmask;
		struct sockaddr ifru_hwaddr;
		short ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct ifmap ifru_map;
		char ifru_slave[IFNAMSIZ];
		char ifru_newname[IFNAMSIZ];
		char *ifru_data;
	} ifr_ifru;
};

#define ifr_name		ifr_ifrn.ifrn_name		/* Interface name */
#define ifr_hwaddr		ifr_ifru.ifru_hwaddr	/* MAC address */
#define	ifr_addr		ifr_ifru.ifru_addr		/* Address */
#define	ifr_dstaddr		ifr_ifru.ifru_dstaddr	/* Other end of p-p lnk */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* Broadcast address */
#define	ifr_netmask		ifr_ifru.ifru_netmask	/* Interface net mask */
#define	ifr_flags		ifr_ifru.ifru_flags		/* Flags */
#define	ifr_metric		ifr_ifru.ifru_ivalue	/* Metric */
#define	ifr_mtu			ifr_ifru.ifru_mtu		/* Mtu */
#define ifr_map			ifr_ifru.ifru_map		/* Device map */
#define ifr_slave		ifr_ifru.ifru_slave		/* Slave device */
#define	ifr_data		ifr_ifru.ifru_data		/* For use by interface */
#define ifr_ifindex		ifr_ifru.ifru_ivalue	/* Interface index*/
#define ifr_bandwidth	ifr_ifru.ifru_ivalue	/* Link bandwidth */
#define ifr_qlen		ifr_ifru.ifru_ivalue	/* Queue length */
#define ifr_newname		ifr_ifru.ifru_newname	/* New name */

/* argument for SIOCGIFCONF ioctl */
struct ifconf {
	int              ifc_len; /* size of buffer */
	union {
		char           *ifcu_buf; /* buffer address */
		struct ifreq   *ifcu_req; /* array of structures */
	} ifc_ifcu;
#define ifc_buf             ifc_ifcu.ifcu_buf       /* Buffer address */
#define ifc_req             ifc_ifcu.ifcu_req       /* Array of structures returned */
};


struct if_nameindex {
	unsigned int if_index;
	char *if_name;
};


#ifdef __cplusplus
extern "C" {
#endif


unsigned int if_nametoindex(const char *ifname);
char *if_indextoname(unsigned int ifindex, char *ifname);
struct if_nameindex *if_nameindex(void);
void if_freenameindex(struct if_nameindex *ptr);


#ifdef __cplusplus
}
#endif


#endif //_LIBPHOENIX_NET_IF_H
