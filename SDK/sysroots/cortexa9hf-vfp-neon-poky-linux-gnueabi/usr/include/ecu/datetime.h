/*
***************************************************************************************
***************************************************************************************
***
***     File: config.h
***
***     Project: Alexandra
***
***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
***
***************************************************************************************
***************************************************************************************


    Subject:

    Document reference:

    Description of this header file:

    Version:


***************************************************************************************
*/


/*
**=====================================================================================
** Header definition
**=====================================================================================
*/
#ifndef DATETIME_H
#define DATETIME_H

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include <ctime>
#include <cstdint>
#include <string>
#include <memory>
#include <mutex>

/*
**=====================================================================================
** Public type definitions
**=====================================================================================
*/
namespace ecu {
namespace lapi {

/**
 *******************************************************************************
 **
 **  @brief Class that handles Date and Time in the Alexandra system
 **
 **  @details Contains a static method to get unix time (UTC)
 **
 *******************************************************************************
 */
//class DateTimeData;

class DateTime
{

public:

   /**
    * @brief Copy-Constructor is deleted!
    */
   DateTime(const DateTime&) = delete;

   /**
    * @brief Assignment-operation is deleted!
    */
   DateTime& operator=(const DateTime&) = delete;

   /**
    * @brief Default constructor is deleted!
    */
   DateTime() = delete;

private:

   virtual ~DateTime();

public:
   /**
    * Get the system time (UTC) in seconds, see systemtime_s()
    *
    * @return Seconds since epoch
    */
   static uint64_t systemtime() { return systemtime_s(); }

   /**
    * Get the system time (UTC) in seconds (since epoch)
    *
    * @return Seconds since epoch
    */
   static uint64_t systemtime_s();

   /**
    * Get the system time (UTC) in milliseconds (since epoch)
    *
    * @return Milliseconds since epoch
    */
   static uint64_t systemtime_ms();

   /**
    * Get the system time (UTC) in microseconds (since epoch)
    *
    * @return Microseconds since epoch
    */
   static uint64_t systemtime_us();

   /**
    * @brief Wait for a valid system time to become available
    *
    * @param timeout_s Timeout in seconds to wait before returning without
    *                  a valid time.
    *                  0 = no timeout, wait indefinitely.
    *
    * @return true if a valid time is detected, false if timeout occured
    */
   static bool wait_for_valid_systemtime(uint64_t timeout_s = 0);

// ----------------------------------------------------------------------------

   /**
    * See time_since_boot_ms()
    *
    * @return see time_since_boot_ms()
    */
   static uint64_t time_since_boot() { return time_since_boot_ms(); }

   /**
    * Gets milliseconds since the system boot
    *
    * @details System in the context is the hypervisor, **not** the linux boot.
    * This time is shared over all hypervisor partitions.
    *
    * @return time as milliseconds since boot
    */
   static uint64_t time_since_boot_ms();

   /**
    * Gets nanoseconds since the system boot
    *
    * @details System in the context is the hypervisor, **not** the linux boot.
    * This time is shared over all hypervisor partitions.
    *
    * @return time as nanoseconds since boot
    */
   static uint64_t time_since_boot_ns();

// ----------------------------------------------------------------------------

   /**
   * Convenient method to convert nanoseconds to milliseconds
   *
   * @param t time in nanoseconds
   *
   * @return time as milliseconds
   */
    static uint64_t convert_time_ms(uint64_t t);

   /**
    * @brief Convert a time_since_boot (ticks (ms)) to systime (UTC (ms))
    *
    * @details Calculate UTC time from a time_since_boot. NOTE! Must be
    *          from the same boot, otherwise the behavior is undefined
    *
    * @param since_boot_ms Time since boot (milliseconds since RT boot)
    *
    * @return UTC time as milliseconds
    */
    static uint64_t convert_boot_to_system_ms(const uint64_t since_boot_ms);

    /**
     * @brief Check if provided time seems to be a valid UTC time
     *
     * @details Currently decided based on if later than 2018-01-01 00:00:00
     *
     * @param epoch EPOCH UTC time in seconds
     *
     * @return true if time seems valid, false otherwise
     */
    static bool is_valid_time_s(uint64_t epoch);

    /**
     * @brief Check if provided time seems to be a valid UTC time
     *
     * @details Currently decided based on if later than 2018-01-01 00:00:00
     *
     * @param epoch EPOCH UTC time in milliseconds
     *
     * @return true if time seems valid, false otherwise
     */
    static bool is_valid_time_ms(uint64_t epoch);

    /**
     * @brief Check if provided time seems to be a valid UTC time
     *
     * @details Currently decided based on if later than 2018-01-01 00:00:00
     *
     * @param epoch EPOCH UTC time in microseconds
     *
     * @return true if time seems valid, false otherwise
     */
    static bool is_valid_time_us(uint64_t epoch);

// ----------------------------------------------------------------------------

    /**
     * @brief Get the unique Boot ID
     *
     * @details The Boot ID is a 32bit random number, extracted from the
     *          system's 128bit UUID boot ID.
     *
     * @param boot_id_str optional UUID formatted boot ID to parse,
     *                    default is to read it from the system
     *
     * @return a boot identifier
     */
    static uint32_t boot_id(const std::string& boot_id_str = std::string());
};


}} // end namespaces


#endif /* DATETIME_H */
