/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: mock_partition.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2021
 ***
 ***
 *******************************************************************************
 *******************************************************************************
 */

#ifndef MOCK_PARTITION_H
#define MOCK_PARTITION_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ecu/partition.h>

using namespace ecu::lapi;

namespace ecu{
namespace lapi{

class MockPartition : public Partition
{
public:
   MOCK_METHOD0(identify_partition, Partitions());
};

} // namespace lapi
} // namespace ecu

#endif //MOCK_PARTITION_H