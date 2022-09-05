/*
***************************************************************************************
***************************************************************************************
***
***     File: ApiRequestData.h
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

    \Subject    Linux API request data for ongoing client calls.

    \Reference

    \Abstract



    \Version Id:


    "$Id: ApiRequestData.h 1284 2014-03-26 08:58:10Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef API_REQUEST_DATA_H_
#define API_REQUEST_DATA_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "client/sre_api.h"
#include "client/sre_con_base_priv.h"
#include "shared/Notifiable.h"
#include <stdint.h>
#include <pthread.h>
#include <map>
/*
**=====================================================================================
** type definitions
**=====================================================================================
*/


/*
**=====================================================================================
** Forward declarations, we are only using pointers to the below types
**=====================================================================================
*/


/**
 *************************************************************************************
 **
 **  @brief  This class holds the data for ongoing requests
 **
 **          This class holds the data for ongoing request, which is the request tag,
 **          the mutex and condition_variable, the subject and event to update when
 **          the call returns and the timeout timer.
 **
 **************************************************************************************
 **/
class ApiRequestData
{
public:

   ApiRequestData(tlv_TagType request = TAG_INVALID, sre::SubjectBase *subject_ptr = NULL, sre::EventBase *event_ptr = NULL, bool subscription = false);

   ~ApiRequestData();

   void notify();

   bool getSubscribe();

   void setEventStatus(int status);


public:

   //! This is the request TAG for the ongoing API call
   tlv_TagType m_request;

   //! This is a pointer to the subject (or its subclass) that handles event notification for this function.
   sre::SubjectBase *m_subject_ptr;

   //! This is a pointer to the event associated with this function / tag.
   sre::EventBase *m_event_ptr;

   bool m_subscription;
};

#endif  // API_REQUEST_DATA_H_

/*************************************************************************************/
/** @}
*********************************** End of file **************************************/
