/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * phoenix/ipsec.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PHOENIX_IPSEC_H_
#define _LIBPHOENIX_PHOENIX_IPSEC_H_

#include <phoenix/pfkeyv2.h>


#define IPSEC_PORT_ANY    0
#define IPSEC_ULPROTO_ANY 255
#define IPSEC_PROTO_ANY   255


enum {
	IPSEC_MODE_ANY = 0,
	IPSEC_MODE_TUNNEL,
	IPSEC_MODE_TRANSPORT /* Not implemented */
};


enum {
	IPSEC_POLICY_IPSEC = 0,
	IPSEC_POLICY_BYPASS,
	IPSEC_POLICY_DISCARD
};


enum {
	IPSEC_DIR_ANY = 0,
	IPSEC_DIR_INBOUND,
	IPSEC_DIR_OUTBOUND
};


enum {
	IPSEC_LEVEL_DEFAULT = 0,
	IPSEC_LEVEL_USE,
	IPSEC_LEVEL_REQUIRE,
	IPSEC_LEVEL_UNIQUE
};

#endif /* _LIBPHOENIX_PHOENIX_IPSEC_H_ */
