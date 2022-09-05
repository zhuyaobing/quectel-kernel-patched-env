/*
***************************************************************************************
***************************************************************************************
***
***     File: NotificationTimer.cpp
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

   \Subject      Linux API Notification timer definition.

   \Reference

   \Abstract

   \Version

   "$Id: NotificationTimer.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"

   \{
***************************************************************************************
*/

/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef NOTIFICATION_TIMER_H_
#define NOTIFICATION_TIMER_H_

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/
#include <stdint.h>

/*
**=====================================================================================
** Forward declarations
**=====================================================================================
*/
namespace freja
{
class Notifiable;
class NotificationTimerImpl;

/**
 **************************************************************************************
 **
 **   @brief Notification timer, that notifies a Notifiable on expiration.
 **
 **  These are the states for the timer:
 **  @dot
 **  digraph {
 **   "IDLE";
 **   "HAVE_HANDLER_NO_TIME";
 **   "HAVE_TIME_NO_HANDLER";
 **   "READY_TO_START";
 **   "RUNNING_PERIODIC";
 **   "RUNNING_SINGLE";
 **
 **   "*" -> "IDLE";
 **   "IDLE" -> "HAVE_HANDLER_NO_TIME" [label="setHandler"];
 **   "IDLE" -> "HAVE_TIME_NO_HANDLER" [label="setTime"];
 **   "HAVE_TIME_NO_HANDLER" -> "READY_TO_START"[label="setHandler"];
 **   "HAVE_HANDLER_NO_TIME" -> "READY_TO_START" [label="setTime"];
 **   "READY_TO_START" -> "RUNNING_PERIODIC" [label="startPeriodic"];
 **   "READY_TO_START" -> "RUNNING_SINGLE" [label="startSingleShot   "];
 **   "READY_TO_START" -> "READY_TO_START" [label="setHandler/setTime"];
 **   "RUNNING_PERIODIC" -> "RUNNING_PERIODIC" [label="setHandler/setTime/PeriodicExpired"];
 **   "RUNNING_PERIODIC" -> "READY_TO_START"  [label="cancel"];
 **   "RUNNING_SINGLE" -> "READY_TO_START"  [label="cancel/SingleShotExpired"];
 **
 ** }
 ** @enddot
 **
 **************************************************************************************
 **/
class NotificationTimer
{
public:
   NotificationTimer();
   ~NotificationTimer();

   void setHandler(Notifiable* notifyable, int id = -1);
   int  getId();
   bool setTimeNs(uint32_t seconds, uint32_t nano_seconds);
   bool setTimeMs(uint32_t seconds, uint32_t milli_seconds);
   bool startPeriodic();
   bool startSingleShot();
   bool cancel();
   bool getTimeToExpiration(uint32_t& seconds, uint32_t& nano_seconds);

private:
   //! Pointer to the actual implementation
   NotificationTimerImpl* priv;
};


} /* namespace freja */
#endif // NOTIFICATION_TIMER_H_

/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
