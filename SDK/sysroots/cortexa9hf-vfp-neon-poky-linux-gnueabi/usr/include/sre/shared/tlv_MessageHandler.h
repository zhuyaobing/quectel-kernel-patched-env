/*
***************************************************************************************
***************************************************************************************
***
***     File: tlv_MessageHandler.h
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

   \Subject    Linux API tlv Manager implementation.

   \Reference

   \Abstract



   \Version Id:


   "$Id: tlv_MessageHandler.h 1334 2014-04-23 10:27:05Z STONERIDGE\PSM $"


@{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef TLV_MESSAGE_HANDLER_H_
#define TLV_MESSAGE_HANDLER_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include "shared/TlvFunctionClass.h"
#include "shared/TransactionManager.h"

#include <map>
/*
**=====================================================================================
** type definitions
**=====================================================================================
*/

/*
**=====================================================================================
** forward declarations
**=====================================================================================
*/

/**
 *************************************************************************************
 **
 ** tlv_MessageHandler
 **
 ** @brief This class defines a general message handler and interface.
 **        It recieves TlvMessages from the ioservice level and distributes them to the
 **        virtual handler interface methods.
 **
 **        This class is overridden to implement the functions particular to the dispatcher, client
 **        and handlers. See tlv_Dispatcher, tlv_IO_Manager etc.
 **
 **
 **************************************************************************************
 **/
class tlv_MessageHandler
{
public:

   tlv_MessageHandler(uint16_t address, freja::TransactionManager *trans_manager_ptr);

   virtual ~tlv_MessageHandler();

   /**
    *************************************************************************************
    **
    **   @brief Virtual interface methods to implement message functionality specific
    **          to subclass
    **
    **
    **************************************************************************************
    **/

   //**************************************************************************************

   virtual freja::TlvMessage* handleIncomingMessage(freja::TlvMessage &request);

protected:

   std::map<uint16_t, TlvFunctionClass*> m_func_map;

   uint16_t m_address;
   freja::TransactionManager *m_trans_manager_ptr;

};

#endif  // TLV_MESSAGE_HANDLER_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
