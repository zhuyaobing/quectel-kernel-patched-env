/*
***************************************************************************************
***************************************************************************************
***
***     File: NotificationQueue.h
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

   "$Id: NotificationQueue.h 1322 2014-04-16 08:36:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/


/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef NOTIFICATION_QUEUE_H_
#define NOTIFICATION_QUEUE_H_

#include "Notifiable.h"

#include "NotificationQueueReadOnly.h"
#include "NotificationQueueWriteOnly.h"

#include <pthread.h>
#include <deque>
#include <stdint.h>

/**
 **************************************************************************************
 **
 **   @brief Queue that may have multiple writers and one reader.
 **          The reader is connected with the Notifiable interface.
 **
 **          The queue may or may non be constructed as thread safe in the meaning
 **          that the access to the underlying implementation may be protected by a
 **          mutex if constructed with use_locking = true
 **
 **     Pseudo code example, using 2 threads.
 **     @code
 ** static int32_t message_queue_id = 0;
 ** static int32_t trans_queue_id = 1;
 ** sem_t message_worker_sem;
 ** sem_t trans_worker_sem;
 **
 ** class NotifyStub : public Notifiable
 ** {
 ** public:
 **    virtual void notify(int32_t param_1, int32_t param_2);
 ** };
 ** void NotifyStub::notify(int32_t param_1, int32_t param_2)
 ** {
 **    if (param_1 == message_queue_id)
 **    {
 **       // release a thread that handles this queue
 **       post_sem(&message_queue_sem);
 **    }
 **    if (param_1 == trans_queue_id)
 **    {
 **       transaction_id = param_2;
 **       cout << "Transaction id = " transaction_id << endl;
 **       post_sem(&trans_queue_sem);
 **    }
 ** }
 **
 ** void* messageWorkerThread(void* my_queue)
 ** {
 **    NotificationQueue<Message*>* my_q = static_cast< NotificationQueue<Message*>* >(my_queue);
 **    while (true)
 **    {
 **       do {
 **          s = sem_wait(&msg_worker_sem);
 **       } while ((s == -1) && (errno == EINTR));
 **       Message* msg_ptr;
 **       if (my_q->dequeue(msg_ptr))
 **       {
 **          // We now have ownership of the message
 **          // Pass this message to the appropriate handler
 **          message_receiver->handleNewMessage(msg_ptr);
 **       }
 **    }
 ** }
 **
 ** void* transWorkerThread(void* my_queue)
 ** {
 **    NotificationQueue<Transaction*>* my_q = static_cast< NotificationQueue<Transaction*>* >(my_queue);
 **    while (true)
 **    {
 **       do {
 **          s = sem_wait(&trans_worker_sem);
 **       } while ((s == -1) && (errno == EINTR));
 **       Transaction* trans_ptr;
 **       if (my_q->dequeue(trans_ptr))
 **       {
 **          // We now have ownership of the transaction
 **          // Pass this message to the appropriate handler
 **          transaction_receiver->handleNewMessage(msg_ptr);
 **       }
 **    }
 ** }
 **
 ** TransactionReceiver::TransactionReciever(out_queue_ptr)
 ** {
 **    NotificationQueueWriteOnly<Transaction*>* m_out_queue_ptr = out_queue_ptr;
 ** }
 ** TransactionReceiver::onNewTransaction()
 ** {
 **    Transaction* t = new Transaction;
 **    int32_t transaction_id = getTransactionContent(t);
 **
 **    // Now the queue has ownership
 **    m_out_queue->enqueue(t, transaction_id);
 ** }
 ** // MessageReceiver is analogue to TransactionReceiver
 **
 ** main()
 ** {
 **    // Who to get the notification messages
 **    NotifyStub owner;
 **
 **    // Queue for incomming messages, with thread safe enqueue and dequeue, since we have multiple threads
 **    NotificationQueue<Message*>* message_queue_ptr = new NotificationQueue<int>(true);
 **    message_queue_ptr->setReader(&owner, message_queue_id);
 **
 **    // Create thread to run message handling
 **    pthread_t thread1;
 **    pthread_create(&thread1, NULL, &messageWorkerThread, message_queue_ptr);
 **
 **    // Message receiver that produces messages for the mesage queue, needs the write end of the queue
 **    NotificationQueueWriteOnly<Message*>* message_queue_wo_ptr = new NotificationQueueWriteOnly<Message*>(*message_queue_ptr);
 **    MessageReceiver message_receiver(*message_queue_wo_ptr);
 **    message_receiver.run();
 **
 **    // Queue for transactions, thread safe
 **    NotificationQueue<Transaction*>* trans_queue_ptr = new NotificationQueue<Transaction*>(true);
 **    trans_queue_ptr->setReader(&owner, trans_queue_id);
 **
 **    // Create thread to run transaction handling
 **    pthread_t thread2;
 **    pthread_create(&thread2, NULL, &transWorkerThread, trans_queue_ptr);
 **
 **    // Transaction producer, needs the write end of the queue
 **    NotificationQueueWriteOnly<Transaction*>* trans_queue_wo_ptr = new NotificationQueueWriteOnly<Transaction*>(*trans_queue_ptr);
 **    TransactionReceiver transaction_receiver(*trans_queue_wo_ptr);
 **    transaction_receiver.run();
 **
 **    // Wait for all to finish with wait, join or select.
 ** }
 **
 **     @endcode
 **************************************************************************************
 **/

namespace freja
{

template<typename T>
class NotificationQueue
{
public:

   NotificationQueue<T>(bool use_locking);
   virtual ~NotificationQueue<T>();

   void setReader(Notifiable* reader_ptr, int32_t queue_id = -1);
   void setQueueId(uint32_t queue_id);
   int32_t getQueueId();

   inline void notifyReader(int32_t notification_param_2 = -1);
   inline void enqueue(T entry, int32_t notification_param_2 = -1, bool do_notify = true);
   inline bool dequeue(T& entry);
   inline std::size_t size();

   std::size_t getHighWaterMark() const;

   // Virtual so that a subclass gets polynorphic and can use e.g. dynamic_cast
   virtual void resetHighWaterMark();

   NotificationQueueReadOnly<T>& getNotificatitionQueueReadOnly();
   NotificationQueueWriteOnly<T>& getNotificatitionQueueWriteOnly();

private:

   inline void lock();
   inline void unlock();

   //! The queue implementation
   std::deque<T> m_q;

   //! Tells if we should use our mutex to lock the queue when we access the queue
   bool m_use_locking;

   //! This is the, what this queue calls the Notifiable::notify() with as param_1
   /*!
     This parameter may be used to resolve the origin queue of the notification
   */
   int32_t m_queue_id;

   //! This is who to notify when something is entered into the queue i.e. the reader.
   Notifiable* m_owner;

   //! The mutex used if thread safety is requested.
   pthread_mutex_t m_mutex;

   //! Keeps track of the maximum number of items that have been in the queue since last call to resetHighWaterMark()
   std::size_t m_high_water;

   NotificationQueueReadOnly<T> m_read_queue_interface;
   NotificationQueueWriteOnly<T> m_write_queue_interface;
};

/**
 **************************************************************************************
 **
 **   @brief Constructor.
 **
 **   @param use_locking If set to true, the enqueue and dequeue will use exclusive
 **                      locking and thus be thread safe.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueue<T>::NotificationQueue(bool use_locking) :
   m_use_locking(use_locking),
   m_queue_id(-1),
   m_owner(0),
   m_high_water(0),
   m_read_queue_interface(*this),
   m_write_queue_interface(*this)
{
   if (m_use_locking)
   {
      pthread_mutex_init(&m_mutex, NULL);
   }
}


/**
 **************************************************************************************
 **
 **   @brief Denstructor.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueue<T>::~NotificationQueue<T>()
{
   if (m_use_locking)
   {
      pthread_mutex_destroy(&m_mutex);
   }
}

/**
 **************************************************************************************
 **
 **   @brief Sets which which Notifyable to notify.
 **
 **   @param reader_ptr This object's notify will be called when a new entry is put
 **                     into the queue.
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
void NotificationQueue<T>::setReader(Notifiable* reader_ptr, int32_t queue_id)
{
   m_owner = reader_ptr;
   m_queue_id = queue_id;
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
void NotificationQueue<T>::setQueueId(uint32_t queue_id)
{
   m_queue_id = queue_id;
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
int32_t NotificationQueue<T>::getQueueId()
{
   return m_queue_id;
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
inline void NotificationQueue<T>::notifyReader(int32_t notification_param_2)
{
   if (m_owner != 0)
   {
      m_owner->notify(m_queue_id, notification_param_2);
   }
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
inline void NotificationQueue<T>::enqueue(T entry, int32_t notification_param_2, bool do_notify)
{
   lock();
   m_q.push_back(entry);
   if (m_q.size() > m_high_water)
   {
      m_high_water = m_q.size();
   }
   unlock();

   if (do_notify && (m_owner != 0))
   {
      m_owner->notify(m_queue_id, notification_param_2);
   }
}


/**
 **************************************************************************************
 **
 **   @brief Pulls an entry out of the queue.
 **
 **   @param entry The entry to retrieve. Use pointers or simple types so that new and
 **                delete do not have to be used.
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
inline bool NotificationQueue<T>::dequeue(T& entry)
{
   bool retval(false);
   lock();
   if (m_q.size() > 0)
   {
      entry = m_q.front();
      m_q.pop_front();
      retval = true;
   }
   unlock();

   return retval;
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
inline std::size_t NotificationQueue<T>::size()
{
   std::size_t s;
   lock();
   s = m_q.size();
   unlock();
   return s;
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
std::size_t NotificationQueue<T>::getHighWaterMark() const
{
   return m_high_water;
}


/**
 **************************************************************************************
 **
 **   @brief Returns a pointer to the read only interface.
 **
 **   @retval Reference to a read interface of the queue. The owner is the
 **           NotificationQueue so you may not delete this pointer.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueueReadOnly<T>& NotificationQueue<T>::getNotificatitionQueueReadOnly()
{
   return m_read_queue_interface;
}


/**
 **************************************************************************************
 **
 **   @brief Returns a pointer to the write only interface.
 **
 **   @retval Reference to a write interface of the queue. The owner is the
 **           NotificationQueue so you may not delete this pointer.
 **
 **************************************************************************************
 **/
template<typename T>
NotificationQueueWriteOnly<T>& NotificationQueue<T>::getNotificatitionQueueWriteOnly()
{
   return m_write_queue_interface;
}


/**
 **************************************************************************************
 **
 **   @brief Resets the max number of entires in the queue.
 **
 **************************************************************************************
 **/
template<typename T>
void NotificationQueue<T>::resetHighWaterMark()
{
   m_high_water = 0;
}


/**
 **************************************************************************************
 **
 **   @brief Locks the queue for reading or writing.
 **
 **************************************************************************************
 **/
template<typename T>
inline void NotificationQueue<T>::lock()
{
   if (m_use_locking)
   {
      pthread_mutex_lock(&m_mutex);
   }
}


/**
 **************************************************************************************
 **
 **   @brief Unlocks the queue after reading or writing.
 **
 **************************************************************************************
 **/
template<typename T>
inline void NotificationQueue<T>::unlock()
{
   if (m_use_locking)
   {
      pthread_mutex_unlock(&m_mutex);
   }
}


} /* namespace freja */
#endif // NOTIFICATION_QUEUE_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
