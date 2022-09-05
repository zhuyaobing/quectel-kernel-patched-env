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


    "$Id: SreDBusWrapper.h 1350 2014-04-30 15:19:11Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

/*
**=====================================================================================
**  This program uses C++ STL and Boost libraries
**
**  Boost is used under the the Boost software license, see
**  http://www.boost.org/LICENSE_1_0.txt
**  This file is heavily based on
**  http://www.boost.org/doc/libs/1_51_0/doc/html/boost_asio/example/fork/daemon.cpp
**  Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
**=====================================================================================
*/

#ifndef SREDBUSWRAPPER_H_
#define SREDBUSWRAPPER_H_

#include <dbus/dbus.h>
#include <stdint.h>

namespace freja
{

/**
 *************************************************************************************
 **
 **   @brief SreDBusWrapper is a wrapper around the dbus API.
 **
 **          Main purpose is to reduce the normal application programmer from all the
 **          quirks and undocumented features of the dbus API.\n
 **          There are 4 or more dbus API libraries and it is very easy to get them mixed up.\n
 **
 **          There are \n
 **          - dbus_message_xxxx\n
 **          - dbus_g_proxy_xxxxx which uses glib bindings and all other glib stuff\n
 **          like g_printerr, g_error_free etc. With little documentation.\n
 **          - gdbus_xxxx\n
 **          - g_dbus_xxx
 **          - ModemManager has its own 2 sets of API functions\n
 **          e.g. mm_modem_get_ports() which uses glib stuff.
 **
 **          Even the dbus connections are different.\n
 **          e.g. DBusConnection compared to DBusGConnection.\n
 **
 **          The SRE_ModemManager is acting as a "proxy server" between the SRE client
 **          application and the DBus ModemManager.\n
 **          The SRE_ModemManager wants to be always listening to the DBus ModemManager
 **          so that it can be informed of signal strength changes of the modem.\n
 **          The DBus ModemManager sends these asynchronous messages as SIGNAL messages.
 **
 **          Most of the DBus API functions and examples are for synchronous usage of
 **          DBus. e.g. Create message, connect and send synchronous message and block
 **          waiting for the reply. The reply when it arrives is all packaged in glib
 **          stuff like glib hash tables in 'c'.
 **
 **          The simplist API D-Bus 1.8.0 has been used.\n
 **          There are lots of recommendations not to use it.\n
 **          It does not require glib libraries.\n
 **          Also most of the other APIs are designed for synchronous usage.\n
 **          i.e. You connect to the dbus and wait for a reply.\n
 **          This can be problematic if you want to receive DBus Signal messages of the
 **          signal strength of the Modem. i.e. Always listening.
 **
 **          Basically the normal application programmer only needs a subset of the
 **          dbus API functions.
 **
 **          <b>Note</b>\n
 **          The DBus ModemManager used is version 1.0. This was a major change and not
 **          backwards compatible to the many old examples on internet.\n
 **          The object path is called /org/freedesktop/ModemManager1 where as the old examples have the
 **          object path as /org/freedesktop/ModemManager\n
 **          The interface is called org.freedesktop.ModemManager1 where as the old examples have the
 **          interface as org.freedesktop.ModemManager.\n
 **          The executeable program in both cases is modemmanager.\n
 **          There are few hints as to what is wrong when nothing works.\n
 **          It is recommended that D-Feet which is a program for "inspecting" managers
 **          on the DBus be used.\n
 **          Also the ModemManager comes with the mmcli program.\n
 **          mmcli -h for help with the ModemManager Command Line Interface program.
 **
 **          <b>Quirk</b>\n
 **          Reading the "Text" and "Number" of the properties of the Sms message works
 **          as expected.\n
 **          The DBus ModemManager sendSms sends all the data as a dictionary of variant.\n
 **          The quirk in the ModemManager implementation is that instead of using
 **          "Text" and "Number" it uses "text" and "number".\n
 **          i.e. Lower case. It won't work otherwise.
 **
 **************************************************************************************
 **/
class SreDBusWrapper
{
public:
   SreDBusWrapper();
   virtual ~SreDBusWrapper();

   static const char * getSignature(DBusMessage* msg);

   static DBusMessage* newMethodCall(const char  *bus_name,
                                     const char  *path,
                                     const char  *interface,
                                     const char  *method);
   static void unrefMessage(DBusMessage   *message);

   static int getMessageType(DBusMessage   *message);
   static const char* getMessageErrorText(DBusMessage* msg);

};

} /* namespace freja */

#endif /* SREDBUSWRAPPER_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
