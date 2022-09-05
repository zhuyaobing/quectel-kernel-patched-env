/*
***************************************************************************************
***************************************************************************************
***
***     File: TlvMessageDataIterator.h
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


   "$Id: TlvMessageDataIterator.h 1350 2014-04-30 15:19:11Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TLV_MESSAGE_DATA_ITERATOR_H_
#define TLV_MESSAGE_DATA_ITERATOR_H_

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

namespace freja
{


class TlvMessageData;


class AlignmentStuff
{
public:
   uint32_t m_alignment;
   uint32_t m_start_pos;
   uint32_t m_prev_item_pos;
   uint32_t m_pre_padding;

   void clear(){
      m_alignment = 0;
      m_start_pos = 0;
      m_prev_item_pos = 0;
      m_pre_padding = 0;
   }


};

/**
 *************************************************************************************
 **
 **   @brief TlvMessageDataIterator is used to append or read data from the
 **          TlvMessageData. It uses a signature to interpret the data.
 **          It is based from DBus. Typical signatures are 'i' for int32
 **          'u' for uint32, s for string etc.
 **           e.g. "us" is a uint32_t and a string.
 **
 **          It is recommended to see the Dbus spec and tutorial.
 **          This implementation supports the following
 **          - 'i' int32_t
 **          - 'u' uint32_t
 **          - 'q' uint16_t
 **          - 'y' uint8_t
 **          - 'b' bool. (Stored in the message as U32)
 **          - 's' string
 **          - 'au' array uint32_t
 **          - 'ay' array uint8_t
 **
 **          When required other parts from the Dbus spec can be implemented
 **          e.g. 'v' for variant, () for struct, {} for key, value pairs,
 **          a{} for dictionary.
 **
 **          Basic format of the data is:
 **            Byte Pos
 **          - [0] endian byte. either 'B' or 'l'
 **          - [1] signature length. 0 - 0xFF
 **          - [2...x] signature characters. e.g. "isqbbi"
 **          - [x + 1] signature termination '\0'
 **          - [x + 2] possible position of first data item depending on data alignment.
 **          - [x + 2 ... y] first aligned data
 **          - [y]     possible position of second data item depending on data alignment.
 **
 **          All TlvMessageData MUST be a multiple of 8 bytes.
 **          All the data items are aligned on their correct alignment.
 **          Alignment is either 1,2,4 or 8.
 **          Alignment of 8 is used for struct and uint64 and is not implemented here.
 **
 **
 **          The TlvMessageDataIterator has been designed to help the developer.
 **          The appendU32, appendU16 and appendU8 have different names instead of
 **          just having the one append function which takes different types.
 **          This forces the developer to use the correct function.
 **          Also it stops ambiguity of for example append(12) is this uint32_t or
 **          uint8_t ?
 **
 **          The initForRead() and initForAppend() functions clear the error code in
 **          the iterator.
 **          All the iterator functions ONLY work if the error code is OK.
 **          This means a series of read functions can occur one after the other
 **          without having to check error codes between each read.
 **          At the end the error code can be read.
 **          A getErrorMessage() function gives added information.
 **
 **          The main philosophy is that the data is either 100% correct or 100% wrong.
 **          The initForRead() does a checkMessageDataValidity() and sets the error code.
 **
 **************************************************************************************
 **/
class TlvMessageDataIterator
{
public:
   enum {
      OK = 0,
      WRONG_MESSAGE_TYPE,
      WRONG_DATA_TYPE,
      SIGNATURES_DIFFERENT,
      OUT_OF_MEMORY,
      NOT_INITIALISED_FOR_APPEND,
      NOT_INITIALISED_FOR_READ,
      MESSAGE_DATA_PTR_IS_NULL,
      EXISTING_MESSAGE_HAS_DIFFERENT_ENDIAN,
      INVALID_MESSAGE_DATA_NOT_MULTIPLE_8,
      INVALID_MESSAGE_DATA_BAD_ENDIAN_CHAR,
      INVALID_MESSAGE_DATA_NO_SIGNATURE_TERMINATOR,
      INVALID_MESSAGE_DATA_LEN_EXCEEDS_VECTOR_SIZE,
      DATA_OUT_OF_RANGE     // Trying to read data outside the vector size.
   };
   enum {
      SIGNATURE_LENGTH_POS = 1,
      SIGNATURE_STRING_START_POS = SIGNATURE_LENGTH_POS + 1
   };


   TlvMessageDataIterator();

   ~TlvMessageDataIterator();

   bool initForRead(const TlvMessageData* tlvData_ptr);

   void initForAppend(TlvMessageData* tlvData_ptr, bool write_as_big_endian = false,
                      bool clear_old_data = true);
   // write_as_big_endian == true means write data as Big Endian in the message
   // otherwise data is written as little endian (default)
   // The append functions take into account the endian being used by the current
   // processor (CPU).
   // Extra parameter for future bool clear_old_data = true (default)
   // Can then append more data to existing data.
   // Must check Endian is correct AND the existing data is valid.

   uint32_t checkMessageDataValidity();
   uint32_t getErrorCode();
   const char *getErrorMessage();

   uint32_t checkSignature(const char *expected_signature);

   uint32_t readString(std::string &value);
   uint32_t readU32(uint32_t &value);
   uint32_t readU16(uint16_t &value);
   uint32_t readU8(uint8_t &value);
   uint32_t readBool(bool &value);
   uint32_t readI32(int32_t &value);
   uint32_t readU32Array(std::vector<uint32_t> &arrayU32);
   uint32_t readU8Array(std::vector<uint8_t> &arrayU8);

   bool hasNext();
   bool next();
   uint32_t readNextString(std::string &value);
   uint32_t readNextU32(uint32_t &value);
   uint32_t readNextU16(uint16_t &value);
   uint32_t readNextU8(uint8_t &value);
   uint32_t readNextBool(bool &value);
   uint32_t readNextI32(int32_t &value);

   uint32_t appendString(const std::string &value);
   uint32_t appendU32(uint32_t value);
   uint32_t appendU16(uint16_t value);
   uint32_t appendU8(uint8_t value);
   uint32_t appendBool(bool value);
   uint32_t appendI32(int32_t value);

   uint32_t appendU32Array(std::vector<uint32_t> &arrayU32);
   uint32_t appendU8Array(std::vector<uint8_t> &arrayU8);


   int32_t getNumberRemainingUnusedBytesInVector();
   int32_t getByteAtPos(int32_t pos); // -1 returned if out of range.
   // Used by unit test.
   void printData();

   const char *getSignature();
   int32_t getSignatureLength();
   int32_t getSignatureCharAtIndex(int32_t idx);

   static int32_t getTypeAlignment(char type_char);

   static void swapEndian(uint16_t &value);
   static void swapEndian(uint32_t &value);
   void fixEndian(uint16_t &value);
   void fixEndian(uint32_t &value);

private:
   void clearAlignmentStuff();
   void calcAscendingAlignments();
   void printAlignmentStuff();
   void moveDataForOneSignatureByte();
   uint32_t appendU32(uint32_t value, const char *sig_char);
   void appendSignature(const char *sig);


   const TlvMessageData *m_tlvData_ptr;
   TlvMessageData *m_tlvData_write_ptr;
   uint32_t m_error;
   bool m_initialised_for_read;
   bool m_initialised_for_append;

   int32_t m_data_start_pos;
   int32_t m_data_write_pos;

   int32_t m_data_read_pos;
   int32_t m_sig_read_pos;

   bool m_message_data_as_big_endian;  // true means write data as Big Endian in the message
   // otherwise data is written as little endian (default)
   bool m_swap_endian; // internal variable decided by the initForRead() and
                       // initForAppend() functions.
                       // Takes into account the endian being used by the current
                       // processor (CPU).

   AlignmentStuff alignment_stuff[4];  // Only 4 possible alignments:- 1,2,4,8.
   uint32_t m_num_ascending_alignments;

   std::string m_error_message;
};

} /* namespace freja */

#endif  // TLV_MESSAGE_DATA_ITERATOR_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
