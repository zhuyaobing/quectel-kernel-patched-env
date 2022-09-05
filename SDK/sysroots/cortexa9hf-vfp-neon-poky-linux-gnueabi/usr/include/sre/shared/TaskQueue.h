/*
***************************************************************************************
***************************************************************************************
***
***     File: TaskQueue.h
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

    \Reference

    \Abstract




    \Version Id:


    "$Id: TaskQueue.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include "shared/NotificationQueue.h"
#include "shared/TaskEntry.h"

namespace freja
{

class TaskQueue : public NotificationQueue<TaskEntry *>
{
public:
   TaskQueue(bool use_locking) :
      NotificationQueue<TaskEntry *>(use_locking)
   {}
};

} /* namespace freja */

#endif /* TASKQUEUE_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
