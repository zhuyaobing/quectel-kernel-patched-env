/*
***************************************************************************************
***************************************************************************************
***
***     File: TlvMessage.h
***
***     Project: SRE Linux API
***
***     Copyright (C) Stoneridge Electronics AB, 1990 - $Date:: 2014#$
***
***     This file is a part of the Stoneridge Common Software Linux API
***     and may only be used in accordance with the terms and conditions stipulated
***     in the agreement contract under which it has been supplied.
***
***************************************************************************************
**************************************************************************************/
/** \file

   \ingroup    SRE Linux API

   \Subject    Linux API message definition.

   \Reference

   \Abstract



   \Version Id:


   "$Id: TlvMessage.h 1355 2014-05-05 15:13:32Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TLV_MESSAGE_H_
#define TLV_MESSAGE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include "Message.h"
#include <stdint.h>

#include "client/sre_api.h"

#include <stdio.h>
#include <string.h>
#include <vector>
#include <ostream>
#include "shared/TlvMessageData.h"


/*
**=====================================================================================
** Macro definitions
**=====================================================================================
*/

#define MAX_SIZE 8192

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

namespace freja
{


class RouterInfo
{
public:

   uint16_t getDestQueueId() const;
   void setDestQueueId(uint16_t internalConnDestQueueId);
   uint16_t getSrcQueueId() const;
   void setSrcQueueId(uint16_t internalConnSrcQueueId);
   uint16_t getDispatcherPrefix() const;
   void setDispatcherPrefix(uint16_t dispatcherPrefix);

private:
   uint16_t m_src_queue_id;
   uint16_t m_dest_queue_id;
   uint16_t m_dispatcher_prefix;
   // For internal use. Not sent to the dispatcher.
   // For Router to route to the correct connection queue. e.g. When we have 2 or more
   // dispatchers or when we have several priority connections to a dispatcher.
   //
   // Also say our new application wants to talk to 4 separate Freja boards running
   // identical software. i.e. We have 4 different connections going to each of
   // the Freja boards connected to the dispatcher on the Freja board.
   // The SRC, DES, and FTAG for each of the functions is identical on each of
   // the Freja boards.
   // By using m_dispatcher_prefix with the SRC tag and using
   // m_dispatcher_prefix with the DST tag will allow our new application
   // to distinguish which device it is talking with.


// TODO add priority_queue_Id here.
};
/**
 *************************************************************************************
 **
 **  @brief TlvMessage Class
 **
 **        The TlvMessage class encapsulates the tlv message header and data in a vector for each.
 **        Fields in the header are accessed using get and set functions.
 **
 **        Data is manipulated using the MessageData class which operates independently from the
 **        TlvMessage class.
 **
 **  @note TlvMessageHeader should have 2 or 4 extra bytes added. These bytes are for use when
 **        having dynamic function / destination name resolution. i.e. They could be a timestamp
 **        of when the manager last started. This timestamp is sent back with the function id and
 **        dest id when first resolving dynamic function / destination name. This time stamp
 **        is then sent with all accesses to the manager using the function id and dest id.
 **        If the manager has restarted and the client does not know about it then the
 **        timestamp on the call from the client to the manager will be different.
 **        The manager will refuse the call and send an error code to tell the client
 **        to do a new dynamic function / destination name resolution.
 **        Maybe just a sequence number controlled by the manager could be used instead of
 **        the timestamp? i.e. Values of 0x0000 and 0xFFFF are invalid.
 **        Everytime the manager is restarted then the sequence number increments.
 **        i.e. sequence number is stored in non vol memory.
 **
 **************************************************************************************
 **/
class TlvMessage : public Message
{
public:
   typedef enum
   {
      ERROR_OK,
      ERROR_LENGTH,
      ERROR_CRC,
      ERROR_HEADER_POINTER
   }MessageError;

   TlvMessage();

   TlvMessage(const TlvMessage &message);

   TlvMessage(const TlvMessage &message, bool create_reply);
   // Just creates the reply tlv_MessageHeader.
   // It does not create any TlvMessageData.
   // If create_reply is true then source and dest tags are switched

   TlvMessage(uint16_t tag, uint16_t dest, uint16_t src, uint16_t status, uint16_t func_tag = TAG_INVALID);

   virtual ~TlvMessage();

   //! @todo: move to private part of object.
   MessageError checkMessage() const;

   std::vector<uint8_t> *takeOwnershipOfData();

   std::vector<uint8_t> *takeOwnershipOfCompleteMessage();

   //! This function does not take ownership of the message internal data.
   std::vector<uint8_t> *getData() const;

   const TlvMessageData * getTlvMessageData();

   void setDataPtr(std::vector<uint8_t> * data_ptr);

   MessageError setCompleteMessageRaw(std::vector<uint8_t> *raw_ptr);


   friend std::ostream& operator<<(std::ostream& os, const TlvMessage& m);

   void updateHeaderCRC();

   void setVersion(uint16_t version);
   void setSource(uint16_t source);
   void setDestination(uint16_t destination);
   void setTag(uint16_t tag);
   void setTransTag(uint16_t tag);
   void setLength(uint16_t length);
   void setStatus(uint16_t status);
   void setFuncTag(uint16_t func_tag);
   void setFuncStatus(uint16_t status);
   void setOrigin(uint16_t origin);
   void setSeq(uint16_t sequence_number);

   uint16_t getVersion() const;
   uint16_t getHeaderCrc() const;
   uint16_t getSource() const;
   uint16_t getDestination() const;
   uint16_t getTag() const;
   uint16_t getTransTag() const;
   uint16_t getLength() const;
   uint16_t getStatus() const;
   uint16_t getFuncTag() const;
   uint16_t getFuncStatus() const;
   uint32_t getOriginSeq() const;
   uint16_t getOrigin() const;
   uint16_t getSeq() const;

   std::size_t headerSize() const;
   std::size_t dataSize() const;


   RouterInfo router_info;
private:
   void setHeaderCrc(uint16_t crc);

   uint16_t getHeaderU16(size_t index) const;
   void setHeaderU16(size_t index, uint16_t data);
   uint16_t calculateHeaderCRC() const;

   std::vector<uint8_t> *m_header_ptr;
   std::vector<uint8_t> *m_data_ptr;

   TlvMessageData m_TlvMessageData;
};

} /* namespace freja */

#endif  // TLV_MESSAGE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
