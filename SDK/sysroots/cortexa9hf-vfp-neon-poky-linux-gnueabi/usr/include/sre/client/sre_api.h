/*
***************************************************************************************
***************************************************************************************
***
***     File: sre_api.h
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


   This header file contains class prototypes, data types, functions and other
   definitions required to use the apilib.


   \Version Id:


   "$Id: sre_api.h 1360 2014-05-06 13:09:14Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef SRE_CON_BASE_H_
#define SRE_CON_BASE_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include <stdint.h>

/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


namespace sre
{
typedef enum
{
   STATUS_OK = 0,
   STATUS_READY_TO_SEND,
   STATUS_REQUEST,
   STATUS_REPLY,
   STATUS_GENERAL_ERROR,
   STATUS_UNKNOWN_FUNCTION,
   STATUS_UNKNOWN_REQUEST_TYPE,
   STATUS_WRONG_DATA_LENGTH,
   STATUS_LIB_NOT_INITIALISED,
   STATUS_LIB_ALREADY_INITIALISED,
   STATUS_NO_REPLY,
   STATUS_FUNCTION_UNAVAILABLE,
   STATUS_DATA_SIZE_ERROR,
   STATUS_DATA_RANGE_ERROR,
   STATUS_STRING_RANGE_ERROR,
   STATUS_STRING_OFFSET_ERROR,
   STATUS_SUBSCRIBE_ERROR,
   STATUS_UNKNOWN_ERROR,
   STATUS_ALREADY_ONGOING_REQUEST,
   STATUS_FAILED_TO_CONNECT,
   STATUS_TIMEOUT,
   STATUS_INVALID_MESSAGE,
   STATUS_INVALID_CALLBACK,
   STATUS_READ_ERROR,
   STATUS_WRITE_ERROR,
   STATUS_INVALID_SOCKET_SERVICE,
   STATUS_DESTINATION_UNAVAILABLE,
   STATUS_SUBJECT_EVENT_ERROR,
   STATUS_SUBSCRIPTION_NOT_AVAILABLE,
   STATUS_SUBSCRIPTION_NOT_FOUND,
   STATUS_INVALID_OUT_QUEUE,
   STATUS_TAG_NOT_FOUND,
   STATUS_NOTIFY_MESSAGE_FAIL,
   STATUS_SUBJECT_CLIENT_POINTER_NULL,
   STATUS_CLIENT_HANDLER_ERROR,
   STATUS_API_INIT_EXCEPTION,

   STATUS_NO_MAPPING_TO_FUNCTOR,


   STATUS_MODEM_INVALID_SMS_NUMBER,
   STATUS_MODEM_NO_MODEM_DETECTED,
   STATUS_MODEM_NO_DBUS_CONNECTION,
   STATUS_MODEM_DBUS_ERROR,
   STATUS_MODEM_NOT_DATA_CONNECTED,


   STATUS_MODEM_DBUS_ERROR_OFFSET = 1000,

   STATUS_DBUS_CONNECTION_ERROR = 2000,
   STATUS_DBUS_WRONG_MESSAGE_TYPE,
   STATUS_DBUS_WRONG_DATA_TYPE,
   STATUS_DBUS_SIGNATURES_DIFFERENT,
   STATUS_DBUS_DATA_READ_CONVERSION_ERROR,

   STATUS_TLV_MESSAGE_DATA_ERROR_OFFSET = 3000,


   /* Do not place anything after 0xFFFF,
    * this enum is cast to U16 when used */
   STATUS_UNKNOWN = 0xFFFF


} Status;


/*
**=====================================================================================
** Other definitions
**=====================================================================================
*/

#define DEFAULT_HOST ("127.0.0.1")
#define DEFAULT_PORT ("5000")


/*
**=====================================================================================
** api basic functions - These function are for initiating the api,
**                        and other activities common to all managers.
**=====================================================================================
*/

/**
 *********************************************************************************************
 **
 **  @brief Function to initialise the API.
 **
 **  @param client_address Address of this client. This number is chosen by the application developer.
 **  @param host_ptr Client host address. Default is 127.0.0.1
 **  @param port_ptr Client port. Default is 5000.
 **  @retval Success or status of the api call.
 **
 **  @remark This function must be called first, before any other API function can be called.
 **
 **********************************************************************************************
 **/
Status api_init(uint16_t client_address, const char* host_ptr, const char* port_ptr);


/**
 *********************************************************************************************
 **
 **  @brief Function to check that the API has been initialised.
 **
 **  @retval True if API is initialised.
 **
 **********************************************************************************************
 **/
bool api_isInitialized();

/**
 *********************************************************************************************
 **
 **  @brief Function to close API and perform orderly removal of the client.
 **
 **  @retval Success or status of function call.
 **
 **  @remark Should always be called when an application shuts down.
 **
 **********************************************************************************************
 **/
Status api_close();

} //namespace sre


/* Soon to be obsolete definitions */
#include "sre_con_base_priv.h"

#endif  // SRE_CON_BASE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
