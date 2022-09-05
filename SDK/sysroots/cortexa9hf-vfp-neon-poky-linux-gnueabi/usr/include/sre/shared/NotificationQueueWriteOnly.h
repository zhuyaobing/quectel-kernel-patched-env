/*
***************************************************************************************
***************************************************************************************
***
***     File: NotificationQueueWriteOnly.h
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

   "$Id: NotificationQueueWriteOnly.h 1322 2014-04-16 08:36:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef NOTIFICATION_QUEUE_WRITE_ONLY_H_
#define NOTIFICATION_QUEUE_WRITE_ONLY_H_

#include <deque> // for size_t


namespace freja
{
/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
template<typename T>
class NotificationQueue;


/**
 **************************************************************************************
 **
 **   @brief Queue that may have multiple writers and one reader. This class holds a
 **          write end of the queue.
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
class NotificationQueueWriteOnly
{
public:
   NotificationQueueWriteOnly<T>(NotificationQueue<T>&queue_ref);

   virtual ~NotificationQueueWriteOnly();

   inline void notifyReader(int32_t notification_param_2 = -1);
   inline void enqueue(T entry, int32_t notification_param_2 = -1, bool do_notify = true);
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
 **   @param queue_ref Reference to the queue to which we should provide a write interface.
 **
 **   @note This class does not actually own the queue. It only holds a reference to it.
 **         This means that if the referenced queue is deleted this interface would have
 **         a dangling reference to it. It is the responsibility of the creator of this
 **         reference to see to it that the write only interface is not used if the
 **         underlying queue is removed.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueueWriteOnly<T>::NotificationQueueWriteOnly(NotificationQueue<T>& queue_ref) :
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
NotificationQueueWriteOnly<T>::~NotificationQueueWriteOnly()
{
}


/**
 **************************************************************************************
 **
 **   @brief Notifies the reader. I.e calls the Notifyable::notify() of reader set in
 **          setReader().
 **
 **          This function may be used when you want to enqueue() several entries and
 **          only want to signal the reader once.
 **
 **   @param notification_param_2 The notify() will be called with this as the
 **                               second notification parameteter of notify()
 **
 **************************************************************************************
 **/
template<typename T>
inline void NotificationQueueWriteOnly<T>::notifyReader(int32_t notification_param_2)
{
   m_queue_ref.notifyReader(notification_param_2);
}


/**
 **************************************************************************************
 **
 **   @brief Puts a new entry into the queue.
 **
 **          This function may be used when you want to enqueue() several entries and
 **          only want to signal the reader once.
 **
 **   @param entry The entry to insert. Use pointers or simple types so that new and
 **                delete do not have to be used.
 **   @param notification_param_2 The notify() will be called with this as the
 **                               second notification parameteter of notify()
 **   @param do_notify Notify the reader when the new entry has been added to the queue.
 **                    If set to false, call notifyReader() when all the wanted elements
 **                    have been enqueued.
 **
 **************************************************************************************
 **/
template<typename T>
inline void NotificationQueueWriteOnly<T>::enqueue(T entry, int32_t notification_param_2, bool do_notify)
{
   m_queue_ref.enqueue(entry, notification_param_2, do_notify);
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
inline std::size_t NotificationQueueWriteOnly<T>::size()
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
std::size_t NotificationQueueWriteOnly<T>::getHighWaterMark() const
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
void NotificationQueueWriteOnly<T>::resetHighWaterMark()
{
   m_queue_ref.resetHighWaterMark();
}


} /* namespace freja */
#endif // NOTIFICATION_QUEUE_WRITE_ONLY_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
