/*
***************************************************************************************
***************************************************************************************
***
***     File: DataFilter.h
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

   \Subject    Linux API Data Filter base class.

   \Reference

   \Abstract



   \Version Id:


   "$Id: DataFilter.h 1261 2014-03-19 07:17:38Z STONERIDGE\jryd $"

\{
***************************************************************************************
*/
#ifndef DATA_FILTER_H_
#define DATA_FILTER_H_

#include <stdint.h>
#include <vector>
#include <list>
#include <ostream>

/**
 *********************************************************************************************
 **
 **  @brief This class is the base class for data filter.
 **
 **         The m_next_filter_ptr pointer is used to create a chain of filters. The input from the
 **         previous filter is inserted to the next filter in the chain until the end is reached.
 **
 **         The result of the chained filters is extracted from the last filter in the chain using
 **         getFilterOutput.
 **
 **         m_data_in_list takes the input to the filter and m_data_out_list is used for the output.
 **
 **         Use incrementalFilter for algorithms that can operate on incomplete chunks of data,
 **         used for read, e.g. byte unstuffing.
 **
 **         directFiltering expects complete data and finishes in one call, used for transmit.
 **
 **********************************************************************************************
 **/
class DataFilter
{
public:
   DataFilter(DataFilter *next_filter_ptr = NULL);

   virtual ~DataFilter();

   void addFilter(DataFilter *filter_ptr);

   bool getFilterOutput(std::vector<uint8_t>** msg_ptr);

   bool getOutData(std::vector<uint8_t>** msg_ptr);

   void insertData(std::vector<uint8_t>* msg_ptr);

   //! Pure virtual to implement incremental filtering function.
   //! Takes data from m_data_in_list and inserts result in m_data_out_list
   //! Responsible for removing msg from m_data_in_list when used.
   virtual void incrementalFilter() = 0;

   //! Pure virtual to implement direct filtering function.
   //! The caller is responsible for deleting the returned buffer
   virtual std::vector<uint8_t> *directFilter(const std::vector<uint8_t> &in_buf) = 0;

   //! Pointer to next filter in chain of filters. Last will be NULL.
   DataFilter *m_next_filter_ptr;

protected:
   //! Queue for input data to filter
   std::list< std::vector<uint8_t>* > m_data_in_list;
   //! Queue for output data from filter
   std::list< std::vector<uint8_t>* > m_data_out_list;

};

#endif // DATA_FILTER_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
