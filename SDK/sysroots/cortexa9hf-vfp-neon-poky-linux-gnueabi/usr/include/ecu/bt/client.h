/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/bt/client.h
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
#ifndef API_ECU_BT_CLIENT_H_
#define API_ECU_BT_CLIENT_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <memory>
#include <ecu/com/client.h>
#include <ecu/bt/discovery/manager.h>

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
 * @brief Bluetooth API for communicating with the shared Bluetooth stack
 *
 * @note Currently the LAPI Bluetooth API only supports device discovery
 *
 * @details Each application should initialize one ITransportClient for all the internal
 * communication and pass that client to the constructor of the BluetoothClient
 * that will then be used for communication with the Bluetooth daemon.
 *
 * ### Discover / scan for Bluetooth LE devices
 *
 * Instantiate a BluetoothClient and get the handle to it's IDiscoveryManager
 * which can then be used to control the scanning.
 *
 * Register an observer to receive the scan results periodically or poll them
 * when needed.
 *
 *******************************************************************************
 */
class BluetoothClient
{
public:
   /**
    ****************************************************************************
    *
    * @brief default constructor is deleted
    *
    ****************************************************************************
    */
   BluetoothClient() = delete;

   /**
    ****************************************************************************
    *
    * @brief Construct a BluetoothClient using the provided ITransportClient_ptr
    *
    ****************************************************************************
    */
   explicit BluetoothClient(com::ITransportClient_ptr tclient);

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~BluetoothClient();

   /**
    ****************************************************************************
    *
    * @brief Get Bluetooth stack availability
    *
    * @details Even though the LAPI Bluetooth API is installed on all systems,
    * the Bluetooth stack is only installed on customer demand and availability
    * should be checked using this method..
    *
    * @note Will block until 5 seconds after BluetoothClient initialization
    * if no status has been received from the BltD.
    *
    * @return true if Bluetooth stack is available (BltD is alive)
    *
    ****************************************************************************
    */
   virtual bool available();

   /**
    ****************************************************************************
    *
    * @brief Get Bluetooth stack operational state
    *
    * @details Check if the Bluetooth stack is available and operational
    *
    * @note Will block until 5 seconds after BluetoothClient initialization
    * if no status has been received from the BltD.
    *
    * @return true if Bluetooth stack is operational
    *
    ****************************************************************************
    */
   virtual bool operational();

   /**
    ****************************************************************************
    *
    * @brief Get a shared pointer to the discovery manager
    *
    ****************************************************************************
    */
   virtual IDiscoveryManager_ptr discovery_manager();


//-----------------------------------------------------------------------------
private:
   com::ITransportClient_ptr m_tclient;
   com::ISubscriptionObserver_ptr m_observer;
   bt::IDiscoveryManager_ptr m_dismgr;
//-----------------------------------------------------------------------------
};

/**
 ****************************************************************************
 *
 * @brief Convenience typedef to a shared pointer type of this class
 *
 ****************************************************************************
 */
using BluetoothClient_ptr = std::shared_ptr<BluetoothClient>;

/**
 *******************************************************************************
 *
 *  @brief Create a Bluetooth Client
 *
 *  @details Creates a new Bluetooth Client using the supplied transport client
 *  Note that there can be only one Bluetooth client per TransportClient
 *
 *  @param tclient An ITransportClient for internal communication
 *
 *  @return BluetoothClient_ptr to the single instance of the Bluetooth client
 *
 *******************************************************************************
 */
BluetoothClient_ptr
create_bt_client(com::ITransportClient_ptr& tclient);

}  // namespace bt
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_BT_CLIENT_H_
