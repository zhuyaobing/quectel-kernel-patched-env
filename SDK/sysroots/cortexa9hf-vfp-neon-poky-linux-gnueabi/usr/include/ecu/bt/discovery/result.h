/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/bt/discovery/result.h
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
#ifndef API_ECU_BT_DISCOVERY_RESULT_H_
#define API_ECU_BT_DISCOVERY_RESULT_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <vector>
#include <ecu/bt/device.h>

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
 * @brief Holds the discovery results of an (ongoing) device scan
 *
 * @details Used by the IDiscoveryManager and IDiscoveryObserver to report
 * discovered BT devices
 *
 *******************************************************************************
 */
class DiscoveryResult
{
public:
   /**
    ****************************************************************************
    *
    * @brief default constructor creates empty DiscoveryResults
    *
    *
    ****************************************************************************
    */
   DiscoveryResult();

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~DiscoveryResult() = default;

   /**
    ****************************************************************************
    *
    * @brief constructor for all constant data
    *
    ****************************************************************************
    */
   DiscoveryResult(
         std::vector<Device>&& devices,
         int                 error,
         uint64_t            timestamp);

   /**
    ****************************************************************************
    *
    * @brief Vector of discovered devices
    *
    ****************************************************************************
    */
   virtual const std::vector<Device>& devices() const;

   /**
    ****************************************************************************
    *
    * @brief Error code of scan
    *
    * @details TBD
    *
    * @return 0 if no error has occurred
    *
    ****************************************************************************
    */
   virtual int error() const;

   /**
    ****************************************************************************
    *
    * @brief Timestamp of when the results was last updated
    *
    * @return timestamp in milliseconds since boot
    *
    ****************************************************************************
    */
   virtual uint64_t timestamp() const;

//------------------------------------------------------------------------------
private:
   std::vector<Device>  m_devices;
   int                  m_error;
   uint64_t             m_timestamp;
//------------------------------------------------------------------------------
};

}  // namespace bt
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_BT_DISCOVERY_RESULT_H_
