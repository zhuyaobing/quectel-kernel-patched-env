/**
 *******************************************************************************
 *******************************************************************************
 ***
 ***  File: channel.h
 ***
 ***  Project: Alexandra
 ***
 ***  Copyright (C) Stoneridge Electronics AB 1990 - 2021
 ***
 ***  The information in this document belongs to Stoneridge, Inc. or its
 ***  subsidiaries and may be confidential. This document and information
 ***  in it may not be copied, reproduced, revealed or appropriated, either
 ***  in whole or in part, without the prior written consent of Stoneridge, Inc.
 ***
 *******************************************************************************
 *******************************************************************************
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <utility>

#include <ecu/common.h>

namespace ecu {
namespace lapi {

template<typename Item>
class Channel
{
public:
   using GetResult = ecu::lapi::Result<Item, bool>;
   Channel() : m_closed(false) {}
   ~Channel() = default;

   bool put(Item&& item)
   {
      if (m_closed) return false;
      std::lock_guard<decltype(m_mutex)> lock(m_mutex);
      m_queue.emplace_back(std::move(item));
      m_cv.notify_one();
      return true;
   }

   GetResult get(bool wait = true)
   {
      using R = GetResult;

      std::unique_lock<decltype(m_mutex)> lock(m_mutex);
      if ( wait )
         m_cv.wait(lock, [&]() { return m_closed.load() || !m_queue.empty(); });

      if ( m_queue.empty() )
         return R(false, false);

      auto result = m_queue.front();
      m_queue.pop_front();
      return R(true, std::move(result));
   }

   void close()
   {
      m_closed = true;
      m_cv.notify_all();
   }

   bool is_closed() { return m_closed.load(); }

protected:

private:
   Channel(const Channel&) = delete;
   Channel(Channel&&) = delete;
   Channel operator=(const Channel&) = delete;

   std::list<Item> m_queue;
   std::mutex m_mutex;
   std::condition_variable m_cv;
   std::atomic_bool m_closed;
};

}} // namespace

#endif /* end of include guard: CHANNEL_H */
