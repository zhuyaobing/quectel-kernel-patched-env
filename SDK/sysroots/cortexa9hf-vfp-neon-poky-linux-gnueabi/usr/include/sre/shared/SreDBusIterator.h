/*
***************************************************************************************
***************************************************************************************
***
***     File: SreDBusIterator.h
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

    \Subject    Linux API daemon implementation.

    \Reference  TBD

    \Abstract




    \Version Id:


    "$Id: SreDBusIterator.h 1355 2014-05-05 15:13:32Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef SREDBUSITERATOR_H_
#define SREDBUSITERATOR_H_

#include "managers/modemmanager/DBusModemManager.h"
#include <stdint.h>
#include <string>

namespace freja
{

/**
 *************************************************************************************
 **
 **   @brief SreDBusIterator is a wrapper around the dbus iterator.
 **
 **          Main purpose is to reduce the normal application programmer from all the
 **          quirks and undocumented features of the dbus iterator.
 **          Basically the normal application programmer only needs a subset of the
 **          dbus iterator functions.
 **
 **          To avoid avoid excessive error checking after reading or appending values
 **          to the DBusMessage an internal m_error code is kept inside the
 **          SreDBusIterator. Reading or appending values to the DBMessage ONLY occur
 **          if m_error == 0.
 **          This means a whole group of values can be read or written to the
 **          DBusMessage and you ONLY need to check at the end to see that m_error == 0.
 **          Usually these bugs are found during development.
 **          The functions getErrorCode() and getErrorMessage() are useful here.
 **
 **************************************************************************************
 **/
class SreDBusIterator
{
public:
   enum {
      OK = 0,
      WRONG_MESSAGE_TYPE,
      WRONG_DATA_TYPE,
      SIGNATURES_DIFFERENT,
      OUT_OF_MEMORY,
      NOT_INITIALISED_FOR_APPEND,
      NOT_INITIALISED_FOR_READ
   };


   virtual ~SreDBusIterator();


   SreDBusIterator();

   bool initForRead(DBusMessage* msg);

   void initForAppend(DBusMessage* msg);

   void readString(std::string &value);
   void readVariantString(std::string &value);

   void appendString(const std::string &value);
   void appendString(const char *str_ptr);

   void appendObjectPath(const std::string &value);
   void appendObjectPath(const char *str_ptr);


   bool hasError();
   uint32_t getErrorCode();
   const char *getErrorMessage();
   uint32_t checkSignature(const char *expected_signature);
   const char *getSignature();
   DBusMessageIter *getDBusMessageIterPtr(){
      return &m_dbus_itr;
   }


private:
   DBusMessageIter m_dbus_itr;
   uint32_t m_error;
   bool m_initialised_for_read;
   bool m_initialised_for_append;

   std::string m_error_message;
};

} /* namespace freja */

#endif /* SREDBUSITERATOR_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
