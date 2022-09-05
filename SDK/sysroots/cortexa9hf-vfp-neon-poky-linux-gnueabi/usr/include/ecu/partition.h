/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: partition.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2021
 ***
 *******************************************************************************
 *******************************************************************************
*/

#ifndef PARTITION_H
#define PARTITION_H

namespace ecu{
namespace lapi{

/**
 *******************************************************************************
 *
 * @brief Available partitions.
 *
 *******************************************************************************
 */

enum Partitions
{
   P_UNKNOWN,
   P_TELE,
   P_TPA
};

/**
 *******************************************************************************
 *
 * @brief Partition API for current partition identification.
 *
 * @details This class provides a method to identify the current partition in
 * which the application is running.
 *
 *******************************************************************************
 */

class Partition
{
public:

   /**
    ****************************************************************************
    *
    * @brief Default destructor, allows inheritance
    *
    ****************************************************************************
    */
   virtual ~Partition() = default;

   /**
   ****************************************************************************

   @brief Identifies the current partition.

   @details It reads the /etc/hostname file and stores the partition type on a
   private class attribute.

   ****************************************************************************
   */
   virtual Partitions identify_partition();
private:
   Partitions m_partition = P_UNKNOWN;
};

//------------------------------------------------------------------------------

}} /* end namespace */

#endif //PARTITION_H
