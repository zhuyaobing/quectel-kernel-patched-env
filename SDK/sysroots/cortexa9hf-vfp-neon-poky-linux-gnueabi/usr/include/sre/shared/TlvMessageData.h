/*
***************************************************************************************
***************************************************************************************
***
***     File: TlvMessageData.h
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

   \Subject    Linux API message data definition.

   \Reference

   \Abstract



   \Version Id:


   "$Id: TlvMessageData.h 1342 2014-04-28 08:04:09Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TLV_MESSAGE_DATA_H_
#define TLV_MESSAGE_DATA_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/

#include "MessageData.h"

#include <stdint.h>
#include <assert.h>

#include <string>
#include <vector>


/*
**=====================================================================================
** Macro definitions
**=====================================================================================
*/

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


/**
 **************************************************************************************
 **
 **   @brief Temporary implementation of legacy format(one of a non dynamic POD).
 **
 **************************************************************************************
 **/

namespace freja
{


class TlvMessageData : public MessageData
{
   friend class TlvMessageDataIterator;
public:


   TlvMessageData();

   ~TlvMessageData();

   std::vector<uint8_t> *takeOwnershipOfData();

   void appendData(const uint8_t *indata_ptr, std::size_t insize);

   void appendDataItem(uint8_t data);

   void appendDataItem(uint16_t data);

   void appendDataItem(uint32_t data);

   void appendDataItem(int data);

   void appendDataItem(const std::string &s);

   bool getData(uint8_t *outdata_ptr, std::size_t outsize);

   uint8_t getU8();

   uint16_t getU16();

   uint32_t getU32();

   int getInt();

   std::string getString();

   void setData(std::vector<uint8_t> * data_ptr);

   void appendEightByteNullBlock();

   int32_t getVectorSize() const;

private:

   std::vector<uint8_t> *m_data_ptr;
};

} /* namespace freja */

#endif  // TLV_MESSAGE_DATA_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
