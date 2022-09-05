/*
********************************************************************************
********************************************************************************
***
***     File: api/ecu/bt/device.h
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
#ifndef API_ECU_BT_DEVICE_H_
#define API_ECU_BT_DEVICE_H_

/**
**==============================================================================
** Imported definitions
**==============================================================================
*/
#include <string>
#include <vector>

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

#ifndef DOXYGEN_IGNORE
// forward declarations, TBI
class Service {};
using Services = std::vector<Service>;
#endif /* ifndef DOXYGEN_IGNORE */


/**
 *******************************************************************************
 *
 * @brief Bluetooth device data
 *
 * @details Holds device specific data for a known BT device
 * (could be connected, paired, discovered etc.)
 *
 *******************************************************************************
 */
class Device
{
public:
   /**
    ****************************************************************************
    *
    * @brief default constructor creates empty Device
    *
    ****************************************************************************
    */
   Device();

   /**
    ****************************************************************************
    *
    * @brief constructor for all constant data
    *
    ****************************************************************************
    */
   Device(
         std::string  address,
         std::string  name,
         Services     services,
         int8_t       rssi,
         bool         paired,
         bool         connected,
         uint64_t     ts_discovered,
         uint64_t     ts_last_seen);

   /**
    ****************************************************************************
    *
    * @brief Make dtor virtual to allow inheritance
    *
    ****************************************************************************
    */
   virtual ~Device() = default;

   /**
    ****************************************************************************
    *
    * @brief Get the BT device address
    *
    ****************************************************************************
    */
   virtual const std::string& address() const;

   /**
    ****************************************************************************
    *
    * @brief Get the BT device name (if available)
    *
    ****************************************************************************
    */
   virtual const std::string& name() const;

   /**
    ****************************************************************************
    *
    * @brief Get a vector of supported BT services
    *
    ****************************************************************************
    */
   virtual const Services& services() const;

   /**
    ****************************************************************************
    *
    * @brief Get current device signal strength (dBm)
    *
    * @return Signal strength, ranging -127 <= N <= +20 dBm
    *
    ****************************************************************************
    */
   virtual int8_t rssi() const;

   /**
    ****************************************************************************
    *
    * @brief Get device pair state
    *
    * @return true if device is paired with the ECU
    *
    ****************************************************************************
    */
   virtual bool paired() const;

   /**
    ****************************************************************************
    *
    * @brief Get device connection state
    *
    * @return true if the device has an active connection with the ECU
    *
    ****************************************************************************
    */
   virtual bool connected() const;

   /**
    ****************************************************************************
    *
    * @brief Get timestamp of when the device was first discovered
    *
    * @return timestamp (in milliseconds since boot)
    *
    ****************************************************************************
    */
   virtual uint64_t timestamp_discovered() const;

   /**
    ****************************************************************************
    *
    * @brief Get timestamp of when the device was last seen
    *
    * @return timestamp (in milliseconds since boot)
    *
    ****************************************************************************
    */
   virtual uint64_t timestamp_last_seen() const;

//------------------------------------------------------------------------------
private:
   std::string  m_address;
   std::string  m_name;
   Services     m_services;
   int8_t       m_rssi;
   bool         m_paired;
   bool         m_connected;
   uint64_t     m_ts_discovered;
   uint64_t     m_ts_last_seen;
//------------------------------------------------------------------------------
};

}  // namespace bt
}  // namespace lapi
}  // namespace ecu

#endif  // API_ECU_BT_DEVICE_H_

