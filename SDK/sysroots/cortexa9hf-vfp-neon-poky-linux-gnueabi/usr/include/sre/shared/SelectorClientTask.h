/*
***************************************************************************************
***************************************************************************************
***
***     File: Selector.h
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


    "$Id: SelectorClientTask.h 1299 2014-04-02 14:01:50Z STONERIDGE\jryd $"


@{
***************************************************************************************
*/
/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/
#ifndef SELECTOR_CLIENT_TASK_H_
#define SELECTOR_CLIENT_TASK_H_

/*
**=====================================================================================
** Imported definitions, local
**=====================================================================================
*/
#include "shared/TaskEntry.h"

/*
**=====================================================================================
** Imported definitions, external
**=====================================================================================
*/
#include <map>
#include <sys/select.h>


namespace freja
{
/**
 **************************************************************************************
 **
 **   @brief This is a small task class that is used to pass information between
 **          the Selector and its clients.
 **
 **************************************************************************************
 **/
class SelectorClientTask : public TaskEntry
{
public:
   enum {
      HANDLE_FD_REGISTRATION = 123,
      HANDLE_READ,
   };
   SelectorClientTask(std::map<int32_t, Entry * >& m_lookup_table, int signal_pipe);

   virtual ~SelectorClientTask();
   void clearAllFd();
   void addToReadFd(int read_fd, Entry* lookup_entry_ptr);
   void removeFromReadFd(int read_fd);
   int getSignalPipe();
   fd_set getReadFdSet();
   int getHighFd();

private:
   //! The read file descriptor set to watch for changes
   fd_set m_read_fds;

   //! The highest file descriptor in the m_read_fds
   int m_high_fd;

   //! Reference to the lookup table of the Selector.
   std::map<int32_t, Entry * >& m_lookup_table;

   //! Pipe that is used for the client to signal that it wants to be run
   int m_signal_pipe;
};

} /* namespace freja */

#endif /* SELECTOR_CLIENT_TASK_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
