/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/bt/discovery/manager.h
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
#ifndef API_ECU_BT_DISCOVERY_MANAGER_H_
#define API_ECU_BT_DISCOVERY_MANAGER_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <memory>
#include <ecu/bt/discovery/observer.h>
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
 * @brief Discovery manager handles the scanning (discovery) of Bluetooth
 * devices. Get the active instance via BluetoothClient::discovery_manager()
 *
 *******************************************************************************
 */
class IDiscoveryManager
{
public:
   /**
    ****************************************************************************
    *
    * @brief default constructor
    *
    ****************************************************************************
    */
   IDiscoveryManager() = default;

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~IDiscoveryManager() = default;

   /**
    ****************************************************************************
    *
    * @brief Start device discovery
    *
    * @details Issue a non blocking call to start device scanning which will be
    * active until stopped by stop_scan()
    *
    * When a scan is running, either register an observer to receive
    * periodic updates of discovered devices or get the current results by
    * calling scan_result()
    *
    * @note Multiple clients (applications) may be scanning for devices at the
    * same time which limits the scanning configuration options.
    *
    * @return true if start request was sent successfully, false otherwise
    *
    ****************************************************************************
    */
   virtual bool start_scan() = 0;

   /**
    ****************************************************************************
    *
    * @brief Stop device discovery
    *
    * @details Issue a non blocking call to stop device scanning
    *
    * Last scan result will be kept and available for retrieval by calling
    * scan_results() even after the scanning has stopped.
    *
    * @note The BT stack may continue scanning if other clients (applications)
    * are still requesting device discovery
    *
    * @return true if stop request was sent successfully, false otherwise
    *
    ****************************************************************************
    */
   virtual bool stop_scan() = 0;

   /**
    ****************************************************************************
    *
    * @brief Check whether device discovery is currently in progress
    *
    * @return true if scanning is ongoing
    *
    ****************************************************************************
    */
   virtual bool scanning() = 0;

   /**
    ****************************************************************************
    *
    * @brief Get current results of scan
    *
    * @return Copy of the current discovery results
    *
    ****************************************************************************
    */
   virtual DiscoveryResult scan_results() = 0;

   /**
    ****************************************************************************
    *
    * @brief Register a discovery observer to receive scan results periodically
    *
    * @param observer to receive scan results
    *
    ****************************************************************************
    */
   virtual void register_observer(IDiscoveryObserver_ptr observer) = 0;

};

/**
 ****************************************************************************
 *
 * @brief Convenience typedef to a shared pointer type of this class
 *
 ****************************************************************************
 */
using IDiscoveryManager_ptr = std::shared_ptr<IDiscoveryManager>;

}  // namespace bt
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_BT_DISCOVERY_MANAGER_H_
