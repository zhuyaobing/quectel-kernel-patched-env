/*
***************************************************************************************
***************************************************************************************
***
***     File: NotificationQueueReadOnly.h
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

   \ingroup      SRE Linux API

   \Subject      Linux API Thread safe notification queue definition.

   \Reference

   \Abstract

   \Version

   "$Id: NotificationQueueReadOnly.h 1322 2014-04-16 08:36:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef NOTIFICATION_QUEUE_READ_ONLY_H_
#define NOTIFICATION_QUEUE_READ_ONLY_H_

#include <deque> // for size_t
#include <stdint.h>

namespace freja
{

template<typename T>
class NotificationQueue;

/**
 **************************************************************************************
 **
 **   @brief Queue that may have multiple writers and one reader. This class holds a
 **          read end of the queue.
 **          The reader is connected with the Notifiable interface.
 **
 **          The queue may or may non be constructed as thread safe in the meaning
 **          that the access to the underlying implementation may be protected by a
 **          mutex if constructed with use_locking = true.
 **
 **          include NotificationQueue.h instead of this file, they are interdependent.
 **
 **************************************************************************************
 **/
template<typename T>
class NotificationQueueReadOnly
{
public:
   NotificationQueueReadOnly<T>(NotificationQueue<T>&read_read_queue_ref);

   virtual ~NotificationQueueReadOnly();

   void setQueueId(uint32_t queue_id);
   int32_t getQueueId();

   inline bool dequeue(T& entry);
   inline std::size_t size();

   std::size_t getHighWaterMark() const;

   // Virtual so that a subclass gets polynorphic and can use e.g. dynamic_cast
   virtual void resetHighWaterMark();

private:
   //! The actual queue
   NotificationQueue<T>& m_queue_ref;
};


/**
 **************************************************************************************
 **
 **   @brief Constructor.
 **
 **   @param queue_ref Reference to the queue to which we should provide a read interface.
 **
 **   @note This class does not actually own the queue. It only holds a reference to it.
 **         This means that if the referenced queue is deleted this interface would have
 **         a dangling reference to it. It is the responsibility of the creator of this
 **         reference to see to it that the read only interface is not used if the
 **         underlying queue is removed.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueueReadOnly<T>::NotificationQueueReadOnly(NotificationQueue<T>& queue_ref) :
   m_queue_ref(queue_ref)
{
}


/**
 **************************************************************************************
 **
 **   @brief Destructor.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueueReadOnly<T>::~NotificationQueueReadOnly()
{
}


/**
 **************************************************************************************
 **
 **   @brief Sets id of this queue.
 **
 **   @param queue_id   This instance of the NotificationQueue will call
 **                     Notifiable::notify() with this notification
 **                     parameter. Can be used to identify the queue when
 **                     several queues notify the same instance of
 **                     Notifiable.
 **
 **************************************************************************************
 **/
template<typename T>
void NotificationQueueReadOnly<T>::setQueueId(uint32_t queue_id)
{
   m_queue_ref.setQueueId(queue_id);
}


/**
 **************************************************************************************
 **
 **   @brief Gets the id of this queue.
 **
 **   @retval The current id of the queue, -1 if no id has been set.
 **
 **************************************************************************************
 **/
template<typename T>
int32_t NotificationQueueReadOnly<T>::getQueueId()
{
   return m_queue_ref.getQueueId();
}


/**
 **************************************************************************************
 **
 **   @brief Pulls an entry out of the queue.
 **
 **   @param entry [out] The entry to retrieve. Use pointers or simple types so that new and
 **                      delete do not have to be used.
 **
 **   @retval true if @a entry has been filled successfully. False if there is nothing
 **           to return
 **
 **   @note This needs to return bool. We would otherwise have to throw an exception
 **         if the queue is empty.
 **
 **************************************************************************************
 **/
template<typename T>
inline bool NotificationQueueReadOnly<T>::dequeue(T& entry)
{
   return m_queue_ref.dequeue(entry);
}


/**
 **************************************************************************************
 **
 **   @brief Returns the number of entires in the queue.
 **
 **   @retval Number of entries in the queue
 **
 **************************************************************************************
 **/
template<typename T>
inline std::size_t NotificationQueueReadOnly<T>::size()
{
   return m_queue_ref.size();
}


/**
 **************************************************************************************
 **
 **   @brief Returns the number max of entires in the queue since last call to
 **          resetHighWaterMark().
 **
 **   @retval The historical high water mark for the queue.
 **
 **************************************************************************************
 **/
template<typename T>
std::size_t NotificationQueueReadOnly<T>::getHighWaterMark() const
{
   return m_queue_ref.getHighWaterMark();
}


/**
 **************************************************************************************
 **
 **   @brief Resets the max number of entires in the queue.
 **
 **************************************************************************************
 **/
template<typename T>
void NotificationQueueReadOnly<T>::resetHighWaterMark()
{
   m_queue_ref.resetHighWaterMark();
}


} /* namespace */
#endif // NOTIFICATION_QUEUE_READ_ONLY_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
