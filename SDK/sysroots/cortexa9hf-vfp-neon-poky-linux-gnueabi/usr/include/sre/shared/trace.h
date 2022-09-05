/*
***************************************************************************************
***************************************************************************************
***
***     File: trace.h
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

   \Subject    Trace file for debug.

   \Reference  TBD

   \Abstract



   \Version Id:


   "$Id: trace.h 1322 2014-04-16 08:36:50Z STONERIDGE\jryd $"

\{
***************************************************************************************
*/
/*
**=====================================================================================
** Double inclusion protection
**=====================================================================================
*/

#ifndef SRE_TRACE_H_
#define SRE_TRACE_H_


#include <iostream>

typedef enum
{
   TRACE_VERBOSE_LEVEL,
   TRACE_TRACE_LEVEL,            // see apache log4j. Finer grained than debug.
   TRACE_DEBUG_LEVEL,
   TRACE_INFO_LEVEL,
   TRACE_ERROR_LEVEL
} TraceLevelType;

/**
 **************************************************************************************
 **
 **   @brief Logging class
 **
 **************************************************************************************
 **/
class Log
{
public:
   static std::ostream& GetStream() { return std::cout; }
   static bool isLevelActive(TraceLevelType level);
   static const char* getTimestamp();
   static void setLevel(TraceLevelType level);

   static TraceLevelType m_level;
};

inline bool Log::isLevelActive(TraceLevelType level)
{
   return (level < m_level);
}


inline void Log::setLevel(TraceLevelType level)
{
   m_level = level;
}


/**
 **************************************************************************************
 **
 **  @brief Function to redirect log
 **  @param File Pointer to file or stdout.
 **
 **************************************************************************************
 **/
void redirectLog(const char *File);

/**
 **************************************************************************************
 **
 **  @brief Function to close log
 **
 **************************************************************************************
 **/
void closeLog();

/**
 **************************************************************************************
 **
 **
 **  Use these macros for debug tracing.
 **
 **  @brief Macros for debug tracing.
 **
 **
 **  The macro is equivalent to:
 **  if(Log::IsLevelActive(TRACE_DEBUG_LEVEL))
 **  {
 **      nothing
 **  }
 **  else Log::GetStream() << "something" << endl;
 **
 **  You supply the "something" << endl; part, including semicolon,
 **  to make it a complete c++ statement.
 **
 **  usage:
 **
 **  TRACE_DEBUG << "something" << endl;
 **
 **
 **************************************************************************************
 **/
#if 0 // Set to 1 to get timestamps but failing tests...
#define TRACE_VERBOSE ((TRACE_VERBOSE_LEVEL < Log::m_level)) ? /* nothing */ : std::cout << Log::getTimestamp() /* << "something" << endl; */
#define TRACE_TRACE   ((TRACE_TRACE_LEVEL < Log::m_level)) ? /* nothing */ : std::cout << Log::getTimestamp() /* << "something" << endl; */
#define TRACE_DEBUG   ((TRACE_DEBUG_LEVEL < Log::m_level)) ? /* nothing */ : std::cout << Log::getTimestamp() /* << "something" << endl; */
#define TRACE_INFO    ((TRACE_INFO_LEVEL < Log::m_level)) ? /* nothing */ : std::cout << Log::getTimestamp()  /* << "something" << endl; */
#define TRACE_ERROR   ((TRACE_ERROR_LEVEL < Log::m_level)) ? /* nothing */ : std::cout << Log::getTimestamp() /* << "something" << endl; */
#else
#define TRACE_VERBOSE (Log::isLevelActive(TRACE_VERBOSE_LEVEL)) ? /* nothing */ : Log::GetStream() /* << "something" << endl; */
#define TRACE_TRACE   (Log::isLevelActive(TRACE_TRACE_LEVEL)) ? /* nothing */ : Log::GetStream() /* << "something" << endl; */
#define TRACE_DEBUG   (Log::isLevelActive(TRACE_DEBUG_LEVEL)) ? /* nothing */ : Log::GetStream() /* << "something" << endl; */
#define TRACE_INFO    (Log::isLevelActive(TRACE_INFO_LEVEL)) ? /* nothing */ : Log::GetStream()  /* << "something" << endl; */
#define TRACE_ERROR   (Log::isLevelActive(TRACE_ERROR_LEVEL)) ? /* nothing */ : Log::GetStream() /* << "something" << endl; */
#endif
#endif //SRE_TRACE_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
