/*
***************************************************************************************
***************************************************************************************
***
***     File: Stack.cpp
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

   \Subject    Linux API Stack.

   \Reference

   \Abstract



   \Version Id:


   "$Id: Stack.h 1274 2014-03-19 14:08:37Z STONERIDGE\jryd $"

\{
***************************************************************************************
*/
#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <vector>

class DataFilter;


/**
 *********************************************************************************************
 **
 **  @brief This class aggregates data filters/processing that is performed before or after
 **         the data becomes a TlvMessage.
 **
 **         It will hold a filter that performs byte stuffing and unstuffing of message data.
 **
 **         Other processing/filtering classes can be added to the stack as needed.
 **
 **********************************************************************************************
 **/
class Stack
{
public:
   Stack();
   Stack(DataFilter *filter_ptr);
   virtual ~Stack();

   void addFilter(DataFilter *filter_ptr);

   bool getOutData(std::vector<uint8_t>** msg_ptr);

   void insertData(std::vector<uint8_t>* msg_ptr);

   std::vector<uint8_t> *directFilter(const std::vector<uint8_t> &in_buf);

private:

   DataFilter *m_filter_ptr;
};

#endif //STACK_H
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
