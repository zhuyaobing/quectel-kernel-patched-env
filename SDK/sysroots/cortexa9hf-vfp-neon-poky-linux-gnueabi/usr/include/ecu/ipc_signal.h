/*
 ***************************************************************************************
 ***************************************************************************************
 ***
 ***    File: ipc_signal.h
 ***
 ***    System: LAPI-IPC
 ***
 ***    Project: Alexandra
 ***
 ***    Copyright (C) Stoneridge Electronics AB, 1990 - 2016
 ***
 ***    The information in this document belongs to Stoneridge, Inc. or its
 ***    subsidiaries, and may be confidential. This document and information
 ***    in it may not be copied, reproduced, revealed or appropriated, either
 ***    in whole or in part, without the prior written consent of Stoneridge, Inc.
 ***
 ***************************************************************************************
 ***************************************************************************************
 */

#ifndef IPC_SIGNAL_H
#define IPC_SIGNAL_H

#include <vector>

#include <ecu/ipc_typedefs.h>

namespace ecu {
namespace lapi {
namespace ipc {

struct IpcSignal
{
   uint32_t id;
   uint8_t status;
   uint64_t timestamp;
   std::vector<char> data;

   friend std::ostream & operator<<( std::ostream &mystream, const IpcSignal &message );
};


/*************************************************************************************
 * IPC signal_t signal struct.
 *
 * statid: Lower 28 bits are dl id.
 *            Upper 4 bits are status flags from dl (if applicable).
 *
 * seq:    Sequence number for transport.
 * length: Total length of message. 1 for non transport.
 * data:   64 bits of data.
 *************************************************************************************/
//typedef struct
//{
   //U32 statid;
   //U16 seq;
   //U16 length;
   //Split64 data;
//} sig_t;
//
//#pragma pack(push ,1)
//struct signal_t
//{
	//U32 statid;
	//U16 seq;
	//U16 length;
	//Split64 data;
//};
//#pragma pack(pop)

template<std::size_t N = 8>
struct signal_t_dyn
{
	U32 statid;
	U16 seq;
	U16 length;
	uint8_t data[N];
};

} // namespace ipc
} // namespace lapi
} // namespace ecu

#endif /* IPC_SIGNAL_H */
