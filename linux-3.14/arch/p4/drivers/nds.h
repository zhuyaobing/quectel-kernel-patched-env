/*
 *  PikeOS "virtual network" drivers message format
 * 
 *  Author: Alex Zuepke <azu@sysgo.de>
 * 
 *  Copyright (C) 2004-2009 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#ifndef _NDS_H_
#define _NDS_H_

/*
 * Net Driver Sync Messages are a used to exchange control information
 * between a PikeOS network server partition and a client partition.
 *
 * In this driver, NDS is only used here to obtain the real NIC's MAC
 * address.
 *
 * The NDS handling is somewhat hacky, as NDS messages are detected by
 * their size. Regular Ethernet frames have a minimum size of 64 chars.
 * sizeof(struct nds_msg) is smaller. See helmut_rx_int() for more.
 *
 * We also still don't have a common header file defining the struct and
 * #defines below, so you must make sure the network server partition uses
 * the same definitions.
 */

/*
 * net driver sync messages - message types
 */
#define NDS_CMD_PING		0x5049	/* "PI" */
#define NDS_CMD_GET_MAC		0x6d67	/* "mg" */
#define NDS_CMD_SET_MAC		0x6d73	/* "ms" */

#define NDS_E_REQ			0x24	/* "$" */
#define NDS_E_ILL			0x3f	/* "?", Operation not supported */
#define NDS_E_OK			0x21	/* "!", Operation OK */
#define NDS_E_BUSY			0x42	/* "B", Busy, try again later */
#define NDS_E_ERR			0x45	/* "E", Operation Error */

/*
 * net driver sync messages - message itself
 * don't let its size grow beyond 64 bytes!
 */
struct nds_msg
{
	u16 type;
	u8 error;
	u8 sequence;
	union
	{
		u8 load[12];	/* maximum load */
		u8 mac[6];
	} u;
};

#endif
