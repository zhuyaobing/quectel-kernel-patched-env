/*
***************************************************************************************
***************************************************************************************
***
***     File: sre-client-base.h
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

   \Subject    Linux API interface definition.

   \Reference

   \Abstract

   This file contains function definitions and data types used to implement managers.


   \Version Id:


   "$Id: sre_client_base.h 1330 2014-04-22 13:07:36Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef SRE_CLIENT_BASE_H_
#define SRE_CLIENT_BASE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "shared/TlvMessage.h"
#include "shared/TlvMessageData.h"

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

/**
 *************************************************************************************
 **
 **  @brief Address enum for managers and dispatcher,
 **         which has address 0.
 **
 **************************************************************************************
 **/
typedef enum
{
   MAN_DISPATCHER = 0,
   MAN_UNKNOWN_CLIENT,
   MAN_GNSS,
   MAN_MODEM,
   MAN_USB,
   MAN_NETWORK,
   MAN_IO,
   MAN_SECURITY,
   MAN_SYSTEM,
   MAN_TEST,
   MAN_CLIENT_SPACE = 256
} tlv_MessageHandlerAddressType;


/*
**=====================================================================================
** Other definitions
**=====================================================================================
*/

/**
 *************************************************************************************
 **
 **   @brief Function to create a message of POD type T.
 **
 **   @param Data the data to append to the message.
 **   @param tag The message tag. It identifies the function and the data.
 **   @param dest Destination of the message.
 **   @param src  Address of the sender of the message.
 **   @param status Status code. Used to set and track status of message, i.e ok or error.
 **
 **   @retval Pointer to created message.
 **
 **************************************************************************************
 **/
template<typename T>
freja::TlvMessage* makePODMessage(const T &Data, uint16_t tag, uint16_t dest, uint16_t src, uint16_t status)
{
   freja::TlvMessage* message_ptr = new freja::TlvMessage(tag, dest, src, status);

   freja::TlvMessageData data;
   data.appendDataItem(Data);

   message_ptr->setDataPtr(data.takeOwnershipOfData());

   return(message_ptr);
}


#endif  // SRE_CLIENT_BASE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
