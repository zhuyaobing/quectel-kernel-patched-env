/*
***************************************************************************************
***************************************************************************************
***
***     File: DBusModemManager.h
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

    \ingroup    SRE Linux API

    \Subject    Linux API daemon implementation.

    \Reference  TBD

    \Abstract




    \Version Id:


    "$Id: SreDBusManager.h 1348 2014-04-30 10:16:41Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/
#ifndef SREDBUSMANAGER_H_
#define SREDBUSMANAGER_H_

#include "shared/LookupCallbackTable.h"
#include "shared/NotificationTimer.h"
#include "shared/TaskQueue.h"
#include "shared/EntryHandler.h"

#include <dbus/dbus.h>
#include <stdint.h>

namespace freja
{

class SreDBusManager : public Notifiable, public EntryHandler
{
public:
   virtual ~SreDBusManager();

   static SreDBusManager* getInstance();
   static DBusConnection* getConnection();

   int init();

   bool send(DBusMessage *message, uint32_t *sequence_number, Entry* lookup_entry_ptr = NULL);
   void flushConnection();

   bool pollDbus(uint32_t max_num_messages_to_read);
   DBusConnection* reConnect();

   void addToCallbackTable(uint32_t key, Entry* lookup_entry_ptr);
   void removeFromCallbackTable(uint32_t key);

   void setOutTaskQueuePtr(TaskQueue * outTaskQueuePtr);
   virtual void notify(int32_t param_1 = -1, int32_t param_2 = -1);
   uint32_t handleEntry(Entry *entry_ptr, void* extra_data_ptr, uint32_t extra_info);

protected:
   SreDBusManager();
private:
   int setSignalMatchInterfaces();
   // see signals from the given interface
   // i.e. add a rule for which messages we want to see

   void handleMessage(DBusMessage* msg);
   void handleMethodReturnMessage(DBusMessage* msg);
   void handleErrorMessage(DBusMessage* msg);
   void handleSignalMessage(DBusMessage* msg);
   void handleMethodCallMessage(DBusMessage* msg);

   NotificationTimer *m_periodic_timer_ptr;

   static SreDBusManager* m_instance;

   static DBusConnection* m_debus_only_conn;
   // Note no 'G' i.e not a DBusGConnection *conn i.e. Does not need glib.
   // See http://www.matthew.ath.cx/misc/dbus

   LookupCallbackTable m_lookup_table;

   TaskQueue * m_out_task_queue_ptr;

   pthread_mutex_t m_mutex;
};

} /* namespace freja */

#endif /* SREDBUSMANAGER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
