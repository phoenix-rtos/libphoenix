/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * phoenix/ethtool.h
 *
 * Copyright 2025 Phoenix Systems
 * Author: Julian Uziembło
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _PHOENIX_ETHTOOL_H_
#define _PHOENIX_ETHTOOL_H_

#include <stdint.h>
#include <net/ethernet.h>

/* ethtool commands */
#define ETHTOOL_GSET          0x00000001
#define ETHTOOL_SSET          0x00000002
#define ETHTOOL_TEST          0x0000001a
#define ETHTOOL_GSTRINGS      0x0000001b
#define ETHTOOL_GSSET_INFO    0x00000037
#define ETHTOOL_GLINKSETTINGS 0x0000004c
#define ETHTOOL_SLINKSETTINGS 0x0000004d

#define ETH_MDIO_SUPPORTS_C22 1
#define ETH_MDIO_SUPPORTS_C45 2

#define ETH_GSTRING_LEN 32

#define BIT_FROM_NAME(name) (1uLL << (ETHTOOL_LINK_MODE##_##name##_##BIT))

#define SUPPORTED_10baseT_Half       BIT_FROM_NAME(10baseT_Half)
#define SUPPORTED_10baseT_Full       BIT_FROM_NAME(10baseT_Full)
#define SUPPORTED_100baseT_Half      BIT_FROM_NAME(100baseT_Half)
#define SUPPORTED_100baseT_Full      BIT_FROM_NAME(100baseT_Full)
#define SUPPORTED_1000baseT_Half     BIT_FROM_NAME(1000baseT_Half)
#define SUPPORTED_1000baseT_Full     BIT_FROM_NAME(1000baseT_Full)
#define SUPPORTED_Autoneg            BIT_FROM_NAME(Autoneg)
#define SUPPORTED_TP                 BIT_FROM_NAME(TP)
#define SUPPORTED_AUI                BIT_FROM_NAME(AUI)
#define SUPPORTED_MII                BIT_FROM_NAME(MII)
#define SUPPORTED_FIBRE              BIT_FROM_NAME(FIBRE)
#define SUPPORTED_BNC                BIT_FROM_NAME(BNC)
#define SUPPORTED_10000baseT_Full    BIT_FROM_NAME(10000baseT_Full)
#define SUPPORTED_Pause              BIT_FROM_NAME(Pause)
#define SUPPORTED_Asym_Pause         BIT_FROM_NAME(Asym_Pause)
#define SUPPORTED_2500baseX_Full     BIT_FROM_NAME(2500baseX_Full)
#define SUPPORTED_Backplane          BIT_FROM_NAME(Backplane)
#define SUPPORTED_1000baseKX_Full    BIT_FROM_NAME(1000baseKX_Full)
#define SUPPORTED_10000baseKX4_Full  BIT_FROM_NAME(10000baseKX4_Full)
#define SUPPORTED_10000baseKR_Full   BIT_FROM_NAME(10000baseKR_Full)
#define SUPPORTED_10000baseR_FEC     BIT_FROM_NAME(10000baseR_FEC)
#define SUPPORTED_20000baseMLD2_Full BIT_FROM_NAME(20000baseMLD2_Full)
#define SUPPORTED_20000baseKR2_Full  BIT_FROM_NAME(20000baseKR2_Full)
#define SUPPORTED_40000baseKR4_Full  BIT_FROM_NAME(40000baseKR4_Full)
#define SUPPORTED_40000baseCR4_Full  BIT_FROM_NAME(40000baseCR4_Full)
#define SUPPORTED_40000baseSR4_Full  BIT_FROM_NAME(40000baseSR4_Full)
#define SUPPORTED_40000baseLR4_Full  BIT_FROM_NAME(40000baseLR4_Full)
#define SUPPORTED_56000baseKR4_Full  BIT_FROM_NAME(56000baseKR4_Full)
#define SUPPORTED_56000baseCR4_Full  BIT_FROM_NAME(56000baseCR4_Full)
#define SUPPORTED_56000baseSR4_Full  BIT_FROM_NAME(56000baseSR4_Full)
#define SUPPORTED_56000baseLR4_Full  BIT_FROM_NAME(56000baseLR4_Full)

#define ADVERTISED_10baseT_Half       BIT_FROM_NAME(10baseT_Half)
#define ADVERTISED_10baseT_Full       BIT_FROM_NAME(10baseT_Full)
#define ADVERTISED_100baseT_Half      BIT_FROM_NAME(100baseT_Half)
#define ADVERTISED_100baseT_Full      BIT_FROM_NAME(100baseT_Full)
#define ADVERTISED_1000baseT_Half     BIT_FROM_NAME(1000baseT_Half)
#define ADVERTISED_1000baseT_Full     BIT_FROM_NAME(1000baseT_Full)
#define ADVERTISED_Autoneg            BIT_FROM_NAME(Autoneg)
#define ADVERTISED_TP                 BIT_FROM_NAME(TP)
#define ADVERTISED_AUI                BIT_FROM_NAME(AUI)
#define ADVERTISED_MII                BIT_FROM_NAME(MII)
#define ADVERTISED_FIBRE              BIT_FROM_NAME(FIBRE)
#define ADVERTISED_BNC                BIT_FROM_NAME(BNC)
#define ADVERTISED_10000baseT_Full    BIT_FROM_NAME(10000baseT_Full)
#define ADVERTISED_Pause              BIT_FROM_NAME(Pause)
#define ADVERTISED_Asym_Pause         BIT_FROM_NAME(Asym_Pause)
#define ADVERTISED_2500baseX_Full     BIT_FROM_NAME(2500baseX_Full)
#define ADVERTISED_Backplane          BIT_FROM_NAME(Backplane)
#define ADVERTISED_1000baseKX_Full    BIT_FROM_NAME(1000baseKX_Full)
#define ADVERTISED_10000baseKX4_Full  BIT_FROM_NAME(10000baseKX4_Full)
#define ADVERTISED_10000baseKR_Full   BIT_FROM_NAME(10000baseKR_Full)
#define ADVERTISED_10000baseR_FEC     BIT_FROM_NAME(10000baseR_FEC)
#define ADVERTISED_20000baseMLD2_Full BIT_FROM_NAME(20000baseMLD2_Full)
#define ADVERTISED_20000baseKR2_Full  BIT_FROM_NAME(20000baseKR2_Full)
#define ADVERTISED_40000baseKR4_Full  BIT_FROM_NAME(40000baseKR4_Full)
#define ADVERTISED_40000baseCR4_Full  BIT_FROM_NAME(40000baseCR4_Full)
#define ADVERTISED_40000baseSR4_Full  BIT_FROM_NAME(40000baseSR4_Full)
#define ADVERTISED_40000baseLR4_Full  BIT_FROM_NAME(40000baseLR4_Full)
#define ADVERTISED_56000baseKR4_Full  BIT_FROM_NAME(56000baseKR4_Full)
#define ADVERTISED_56000baseCR4_Full  BIT_FROM_NAME(56000baseCR4_Full)
#define ADVERTISED_56000baseSR4_Full  BIT_FROM_NAME(56000baseSR4_Full)
#define ADVERTISED_56000baseLR4_Full  BIT_FROM_NAME(56000baseLR4_Full)

#define SPEED_10      10
#define SPEED_100     100
#define SPEED_1000    1000
#define SPEED_2500    2500
#define SPEED_5000    5000
#define SPEED_10000   10000
#define SPEED_14000   14000
#define SPEED_20000   20000
#define SPEED_25000   25000
#define SPEED_40000   40000
#define SPEED_50000   50000
#define SPEED_56000   56000
#define SPEED_100000  100000
#define SPEED_200000  200000
#define SPEED_400000  400000
#define SPEED_800000  800000
#define SPEED_UNKNOWN -1

#define DUPLEX_HALF    0x00
#define DUPLEX_FULL    0x01
#define DUPLEX_UNKNOWN 0xff

#define PORT_TP    0x00
#define PORT_AUI   0x01
#define PORT_MII   0x02
#define PORT_FIBRE 0x03
#define PORT_BNC   0x04
#define PORT_DA    0x05
#define PORT_NONE  0xef
#define PORT_OTHER 0xff

#define AUTONEG_DISABLE 0x00
#define AUTONEG_ENABLE  0x01

#define ETH_TP_MDI_INVALID 0x00
#define ETH_TP_MDI         0x01
#define ETH_TP_MDI_X       0x02
#define ETH_TP_MDI_AUTO    0x03


enum {
	ETHTOOL_LINK_MODE_10baseT_Half_BIT,
	ETHTOOL_LINK_MODE_10baseT_Full_BIT,
	ETHTOOL_LINK_MODE_100baseT_Half_BIT,
	ETHTOOL_LINK_MODE_100baseT_Full_BIT,
	ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
	ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
	ETHTOOL_LINK_MODE_Autoneg_BIT,
	ETHTOOL_LINK_MODE_TP_BIT,
	ETHTOOL_LINK_MODE_AUI_BIT,
	ETHTOOL_LINK_MODE_MII_BIT,
	ETHTOOL_LINK_MODE_FIBRE_BIT,
	ETHTOOL_LINK_MODE_BNC_BIT,
	ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
	ETHTOOL_LINK_MODE_Pause_BIT,
	ETHTOOL_LINK_MODE_Asym_Pause_BIT,
	ETHTOOL_LINK_MODE_2500baseX_Full_BIT,
	ETHTOOL_LINK_MODE_Backplane_BIT,
	ETHTOOL_LINK_MODE_1000baseKX_Full_BIT,
	ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT,
	ETHTOOL_LINK_MODE_10000baseKR_Full_BIT,
	ETHTOOL_LINK_MODE_10000baseR_FEC_BIT,
	ETHTOOL_LINK_MODE_20000baseMLD2_Full_BIT,
	ETHTOOL_LINK_MODE_20000baseKR2_Full_BIT,
	ETHTOOL_LINK_MODE_40000baseKR4_Full_BIT,
	ETHTOOL_LINK_MODE_40000baseCR4_Full_BIT,
	ETHTOOL_LINK_MODE_40000baseSR4_Full_BIT,
	ETHTOOL_LINK_MODE_40000baseLR4_Full_BIT,
	ETHTOOL_LINK_MODE_56000baseKR4_Full_BIT,
	ETHTOOL_LINK_MODE_56000baseCR4_Full_BIT,
	ETHTOOL_LINK_MODE_56000baseSR4_Full_BIT,
	ETHTOOL_LINK_MODE_56000baseLR4_Full_BIT,
	ETHTOOL_LINK_MODE_25000baseCR_Full_BIT
};

enum ethtool_stringset {
	ETH_SS_TEST = 0,
	ETH_SS_STATS,
	ETH_SS_PRIV_FLAGS,
	ETH_SS_NTUPLE_FILTERS,
	ETH_SS_FEATURES,
	ETH_SS_RSS_HASH_FUNCS,
	ETH_SS_TUNABLES,
	ETH_SS_PHY_STATS,
	ETH_SS_PHY_TUNABLES,
	ETH_SS_LINK_MODES,
	ETH_SS_MSG_CLASSES,
	ETH_SS_WOL_MODES,
	ETH_SS_SOF_TIMESTAMPING,
	ETH_SS_TS_TX_TYPES,
	ETH_SS_TS_RX_FILTERS,
	ETH_SS_UDP_TUNNEL_TYPES,
	ETH_SS_STATS_STD,
	ETH_SS_STATS_ETH_PHY,
	ETH_SS_STATS_ETH_MAC,
	ETH_SS_STATS_ETH_CTRL,
	ETH_SS_STATS_RMON,
	ETH_SS_STATS_PHY,
	ETH_SS_TS_FLAGS,

	ETH_SS_COUNT
};

enum {
	ETH_TEST_FL_OFFLINE = (1u << 0),
	ETH_TEST_FL_FAILED = (1u << 1),
	ETH_TEST_FL_EXTERNAL_LB = (1u << 2),
	ETH_TEST_FL_EXTERNAL_LB_DONE = (1u << 3),
};

struct ethtool_cmd {
	uint32_t cmd;
	uint32_t supported;
	uint32_t advertising;
	uint16_t speed;
	uint8_t duplex;
	uint8_t port;
	uint8_t phy_address;
	uint8_t transceiver;
	uint8_t autoneg;
	uint8_t mdio_support;
	uint32_t maxtxpkt;
	uint32_t maxrxpkt;
	uint16_t speed_hi;
	uint8_t eth_tp_mdix;
	uint8_t eth_tp_mdix_ctrl;
	uint32_t lp_advertising;
	uint32_t reserved[2];
};

struct ethtool_gstrings {
	uint32_t cmd;
	uint32_t string_set;
	uint32_t len;
	uint8_t data[];
};

struct ethtool_sset_info {
	uint32_t cmd;
	uint32_t reserved;
	uint64_t sset_mask;
	uint32_t data[];
};

struct ethtool_test {
	uint32_t cmd;
	uint32_t flags;
	uint32_t reserved;
	uint32_t len;
	uint64_t data[];
};

struct ethtool_link_settings {
	uint32_t cmd;
	uint32_t speed;
	uint8_t duplex;
	uint8_t port;
	uint8_t phy_address;
	uint8_t autoneg;
	uint8_t mdio_support;
	uint8_t eth_tp_mdix;
	uint8_t eth_tp_mdix_ctrl;
	int8_t link_mode_masks_nwords;
	uint8_t transceiver;
	uint8_t master_slave_cfg;
	uint8_t master_slave_state;
	uint8_t rate_matching;
	uint32_t reserved[7];
	uint32_t link_mode_masks[];
};

#endif /* _PHOENIX_ETHTOOL_H_ */
