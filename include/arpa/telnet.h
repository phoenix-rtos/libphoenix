/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arpa/telnet.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARPA_TELNET_H_
#define _LIBPHOENIX_ARPA_TELNET_H_

#define IAC     255
#define DONT    254
#define DO      253
#define WONT    252
#define WILL    251
#define SB      250
#define GA      249
#define EL      248
#define EC      247
#define AYT     246
#define AO      245
#define IP      244
#define BREAK   243
#define DM      242
#define NOP     241
#define SE      240
#define EOR     239
#define ABORT   238
#define SUSP    237
#define xEOF    236

#define SYNCH   242

#define TELOPT_BINARY            0
#define TELOPT_ECHO              1
#define TELOPT_RCP               2
#define TELOPT_SGA               3
#define TELOPT_NAMS              4
#define TELOPT_STATUS            5
#define TELOPT_TM                6
#define TELOPT_RCTE              7
#define TELOPT_NAOL              8
#define TELOPT_NAOP              9
#define TELOPT_NAOCRD           10
#define TELOPT_NAOHTS           11
#define TELOPT_NAOHTD           12
#define TELOPT_NAOFFD           13
#define TELOPT_NAOVTS           14
#define TELOPT_NAOVTD           15
#define TELOPT_NAOLFD           16
#define TELOPT_XASCII           17
#define TELOPT_LOGOUT           18
#define TELOPT_BM               19
#define TELOPT_DET              20
#define TELOPT_SUPDUP           21
#define TELOPT_SUPDUPOUTPUT     22
#define TELOPT_SNDLOC           23
#define TELOPT_TTYPE            24
#define TELOPT_EOR              25
#define TELOPT_TUID             26
#define TELOPT_OUTMRK           27
#define TELOPT_TTYLOC           28
#define TELOPT_3270REGIME       29
#define TELOPT_X3PAD            30
#define TELOPT_NAWS             31
#define TELOPT_TSPEED           32
#define TELOPT_LFLOW            33
#define TELOPT_LINEMODE         34
#define TELOPT_XDISPLOC         35
#define TELOPT_OLD_ENVIRON      36
#define TELOPT_AUTHENTICATION   37
#define TELOPT_ENCRYPT          38
#define TELOPT_NEW_ENVIRON      39
#define TELOPT_EXOPL           255


#endif
