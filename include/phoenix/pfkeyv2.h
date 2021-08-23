/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * phoenix/pfkeyv2.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PHOENIX_PFKEYV2_H_
#define _LIBPHOENIX_PHOENIX_PFKEYV2_H_

#include <stdint.h>


#define PF_KEY_V2 2


struct sadb_msg {
	uint8_t sadb_msg_version;
	uint8_t sadb_msg_type;
	uint8_t sadb_msg_errno;
	uint8_t sadb_msg_satype;
	uint16_t sadb_msg_len;
	uint16_t sadb_msg_reserved;
	uint32_t sadb_msg_seq;
	uint32_t sadb_msg_pid;
} __attribute__((packed));


struct sadb_ext {
	uint16_t sadb_ext_len;
	uint16_t sadb_ext_type;
} __attribute__((packed));


struct sadb_sa {
	uint16_t sadb_sa_len;
	uint16_t sadb_sa_exttype;
	uint32_t sadb_sa_spi;
	uint8_t sadb_sa_replay;
	uint8_t sadb_sa_state;
	uint8_t sadb_sa_auth;
	uint8_t sadb_sa_encrypt;
	uint32_t sadb_sa_flags;
} __attribute__((packed));


struct sadb_lifetime {
	uint16_t sadb_lifetime_len;
	uint16_t sadb_lifetime_exttype;
	uint32_t sadb_lifetime_allocations;
	uint64_t sadb_lifetime_bytes;
	uint64_t sadb_lifetime_addtime;
	uint64_t sadb_lifetime_usetime;
} __attribute__((packed));


struct sadb_address {
	uint16_t sadb_address_len;
	uint16_t sadb_address_exttype;
	uint8_t sadb_address_proto;
	uint8_t sadb_address_prefixlen;
	uint16_t sadb_address_reserved;
} __attribute__((packed));


struct sadb_key {
	uint16_t sadb_key_len;
	uint16_t sadb_key_exttype;
	uint16_t sadb_key_bits;
	uint16_t sadb_key_reserved;
} __attribute__((packed));


struct sadb_ident {
	uint16_t sadb_ident_len;
	uint16_t sadb_ident_exttype;
	uint16_t sadb_ident_type;
	uint16_t sadb_ident_reserved;
	uint64_t sadb_ident_id;
} __attribute__((packed));


struct sadb_sens {
	uint16_t sadb_sens_len;
	uint16_t sadb_sens_exttype;
	uint32_t sadb_sens_dpd;
	uint8_t sadb_sens_sens_level;
	uint8_t sadb_sens_sens_len;
	uint8_t sadb_sens_integ_level;
	uint8_t sadb_sens_integ_len;
	uint32_t sadb_sens_reserved;
} __attribute__((packed));


struct sadb_prop {
	uint16_t sadb_prop_len;
	uint16_t sadb_prop_exttype;
	uint8_t sadb_prop_replay;
	uint8_t sadb_prop_reserved[3];
} __attribute__((packed));


struct sadb_supported {
	uint16_t sadb_supported_len;
	uint16_t sadb_supported_exttype;
	uint32_t sadb_supported_reserved;
} __attribute__((packed));


struct sadb_alg {
	uint8_t sadb_alg_id;
	uint8_t sadb_alg_ivlen;
	uint16_t sadb_alg_minbits;
	uint16_t sadb_alg_maxbits;
	uint16_t sadb_alg_reserved;
} __attribute__((packed));


struct sadb_spirange {
	uint16_t sadb_spirange_len;
	uint16_t sadb_spirange_exttype;
	uint32_t sadb_spirange_min;
	uint32_t sadb_spirange_max;
	uint32_t sadb_spirange_reserved;
} __attribute__((packed));


struct sadb_x_kmprivate {
	uint16_t sadb_x_kmprivate_len;
	uint16_t sadb_x_kmprivate_exttype;
	uint32_t sadb_x_kmprivate_reserved;
} __attribute__((packed));


struct sadb_x_sa2 {
	uint16_t sadb_x_sa2_len;
	uint16_t sadb_x_sa2_exttype;
	uint8_t sadb_x_sa2_mode;
	uint8_t sadb_x_sa2_reserved1;
	uint16_t sadb_x_sa2_reserved2;
	uint32_t sadb_x_sa2_sequence;
	uint32_t sadb_x_sa2_reqid;
} __attribute__((packed));


struct sadb_x_policy {
	uint16_t sadb_x_policy_len;
	uint16_t sadb_x_policy_exttype;
	uint16_t sadb_x_policy_type;
	uint8_t sadb_x_policy_dir;
	uint8_t sadb_x_policy_reserved;
	uint32_t sadb_x_policy_id;
	uint32_t sadb_x_policy_reserved2;
} __attribute__((packed));


struct sadb_x_ipsecrequest {
	uint16_t sadb_x_ipsecrequest_len;
	uint16_t sadb_x_ipsecrequest_proto;
	uint8_t sadb_x_ipsecrequest_mode;
	uint8_t sadb_x_ipsecrequest_level;
	uint16_t sadb_x_ipsecrequest_reqid;
} __attribute__((packed));


struct sadb_x_nat_t_type {
	uint16_t sadb_x_nat_t_type_len;
	uint16_t sadb_x_nat_t_type_exttype;
	uint8_t sadb_x_nat_t_type_type;
	uint8_t sadb_x_nat_t_type_reserved[3];
} __attribute__((packed));


struct sadb_x_nat_t_port {
	uint16_t sadb_x_nat_t_port_len;
	uint16_t sadb_x_nat_t_port_exttype;
	uint16_t sadb_x_nat_t_port_port;
	uint16_t sadb_x_nat_t_port_reserved;
} __attribute__((packed));


struct sadb_x_nat_t_frag {
	uint16_t sadb_x_nat_t_frag_len;
	uint16_t sadb_x_nat_t_frag_exttype;
	uint16_t sadb_x_nat_t_frag_fraglen;
	uint16_t sadb_x_nat_t_frag_reserved;
} __attribute__((packed));


/* Message types */
#define SADB_RESERVED            0
#define SADB_GETSPI              1
#define SADB_UPDATE              2
#define SADB_ADD                 3
#define SADB_DELETE              4
#define SADB_GET                 5
#define SADB_ACQUIRE             6
#define SADB_REGISTER            7
#define SADB_EXPIRE              8
#define SADB_FLUSH               9
#define SADB_DUMP                10
#define SADB_X_PROMISC           11
#define SADB_X_PCHANGE           12
#define SADB_X_SPDUPDATE         13
#define SADB_X_SPDADD            14
#define SADB_X_SPDDELETE         15
#define SADB_X_SPDGET            16
#define SADB_X_SPDACQUIRE        17
#define SADB_X_SPDDUMP           18
#define SADB_X_SPDFLUSH          19
#define SADB_X_SPDSETIDX         20
#define SADB_X_SPDEXPIRE         21
#define SADB_X_SPDDELETE2        22
#define SADB_X_NAT_T_NEW_MAPPING 23
#define SADB_MAX                 23


/* Security Association flags */
#define SADB_SAFLAGS_PFS 1


/* Security Association states */
#define SADB_SASTATE_LARVAL 0
#define SADB_SASTATE_MATURE 1
#define SADB_SASTATE_DYING  2
#define SADB_SASTATE_DEAD   3
#define SADB_SASTATE_MAX    3


/* Security Association types */
#define SADB_SATYPE_UNSPEC         0
#define SADB_SATYPE_AH             2
#define SADB_SATYPE_ESP            3
#define SADB_SATYPE_RSVP           5
#define SADB_SATYPE_OSPFV2         6
#define SADB_SATYPE_RIPV2          7
#define SADB_SATYPE_MIP            8
#define SADB_X_SATYPE_IPCOMP       9
#define SADB_X_SATYPE_TCPSIGNATURE 11
#define SADB_SATYPE_MAX            11


/* Authentication algorithms */
#define SADB_AALG_NONE       0
#define SADB_AALG_MD5HMAC    2
#define SADB_AALG_SHA1HMAC   3
#define SADB_X_AALG_SHA2_256 5
#define SADB_AALG_MAX        5


/* Encryption algorithms */
#define SADB_EALG_NONE    0
#define SADB_EALG_DESCBC  2
#define SADB_EALG_3DESCBC 3
#define SADB_EALG_NULL    11
#define SADB_X_EALG_AES   12
#define SADB_EALG_MAX     250


/* Compression algorithms */
#define SADB_X_CALG_NONE    0
#define SADB_X_CALG_OUI     1
#define SADB_X_CALG_DEFLATE 2
#define SADB_X_CALG_LZS     3
#define SADB_X_CALG_MAX     4


/* Extension Header values */
#define SADB_EXT_RESERVED          0
#define SADB_EXT_SA                1
#define SADB_EXT_LIFETIME_CURRENT  2
#define SADB_EXT_LIFETIME_HARD     3
#define SADB_EXT_LIFETIME_SOFT     4
#define SADB_EXT_ADDRESS_SRC       5
#define SADB_EXT_ADDRESS_DST       6
#define SADB_EXT_ADDRESS_PROXY     7
#define SADB_EXT_KEY_AUTH          8
#define SADB_EXT_KEY_ENCRYPT       9
#define SADB_EXT_IDENTITY_SRC      10
#define SADB_EXT_IDENTITY_DST      11
#define SADB_EXT_SENSITIVITY       12
#define SADB_EXT_PROPOSAL          13
#define SADB_EXT_SUPPORTED_AUTH    14
#define SADB_EXT_SUPPORTED_ENCRYPT 15
#define SADB_EXT_SPIRANGE          16
#define SADB_X_EXT_KMPRIVATE       17
#define SADB_X_EXT_POLICY          18
#define SADB_X_EXT_SA2             19
#define SADB_X_EXT_NAT_T_TYPE      20
#define SADB_X_EXT_NAT_T_SPORT     21
#define SADB_X_EXT_NAT_T_DPORT     22
#define SADB_X_EXT_NAT_T_OA        23
#define SADB_X_EXT_NAT_T_OAI       23
#define SADB_X_EXT_NAT_T_OAR       24
#define SADB_X_EXT_NAT_T_FRAG      25
#define SADB_EXT_MAX               25


/* Identity Extension values */
#define SADB_IDENTTYPE_RESERVED 0
#define SADB_IDENTTYPE_PREFIX   1
#define SADB_IDENTTYPE_FQDN     2
#define SADB_IDENTTYPE_USERFQDN 3
#define SADB_IDENTTYPE_MAX      3

#endif /* _LIBPHOENIX_PHOENIX_PFKEYV2_H_ */
