/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/bt/discovery/observer.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
***
***  The information in this document belongs to Stoneridge, Inc. or its
***  subsidiaries and may be confidential. This document and information
***  in it may not be copied, reproduced, revealed or appropriated, either
***  in whole or in part, without the prior written consent of Stoneridge, Inc.
***
********************************************************************************
********************************************************************************
*/

/**
**==============================================================================
** Header definition
**==============================================================================
*/
#ifndef API_ECU_BT_DISCOVERY_OBSERVER_H_
#define API_ECU_BT_DISCOVERY_OBSERVER_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <memory>
#include <ecu/bt/discovery/result.h>

/**
 *==============================================================================
 * Public type definitions
 *==============================================================================
 */
namespace ecu {
namespace lapi {

/**
 *******************************************************************************
 *
 * @brief Contains Bluetooth related artifacts
 *
 *******************************************************************************
 */
namespace bt {

/**
 *******************************************************************************
 *
 * @brief Observer interface for subscribers of discovery results
 *
 * @details Observer to receive periodic callbacks from the IDiscoveryManager.
 *
 *******************************************************************************
 */
class IDiscoveryObserver
{
public:
   /**
    ****************************************************************************
    *
    * @brief default constructor
    *
    ****************************************************************************
    */
   IDiscoveryObserver() = default;

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~IDiscoveryObserver() = default;

   /**
    ****************************************************************************
    *
    * @brief Callback periodically while a scan is in progress
    *
    * @details This function is pure virtual and has to be overridden by derived
    *          classes implementing this abstract class/interface.
    *
    * @param result Current scan results (ref. only valid during callback)
    *
    ****************************************************************************
    */
   virtual void scan_result(const DiscoveryResult& result) = 0;
};

/**
 ****************************************************************************
 *
 * @brief Convenience typedef to a shared pointer type of this class
 *
 ****************************************************************************
 */
using IDiscoveryObserver_ptr = std::shared_ptr<IDiscoveryObserver>;


}  // namespace bt
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_BT_DISCOVERY_OBSERVER_H_
