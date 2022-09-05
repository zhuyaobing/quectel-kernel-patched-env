/*
***************************************************************************************
***************************************************************************************
***
***     File: BytestuffingFilter.cpp
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

   \Subject    Linux API Byte Stuffing Filter.

   \Reference

   \Abstract



   \Version Id:


   "$Id: BytestuffingFilter.h 1265 2014-03-19 10:37:02Z STONERIDGE\jryd $"

\{
***************************************************************************************
*/
#ifndef BYTESTUFFING_FILTER_H_
#define BYTESTUFFING_FILTER_H_

#include "DataFilter.h"

#include <stdint.h>
#include <vector>


struct BytestuffingState;

class BytestuffingFilter : public DataFilter
{
public:
   BytestuffingFilter();
   virtual ~BytestuffingFilter();

   //! @todo rename and pass actual parameters, to avoid use of side effects.
   virtual void incrementalFilter();

   virtual std::vector<uint8_t> *directFilter(const std::vector<uint8_t> &in_buf);

   //! Start of message byte
   static const uint8_t STX;
   //! End of message byte
   static const uint8_t ETX;
   //! Escape byte
   static const uint8_t ESC;

   bool unstuff(uint8_t next_byte);

   bool stuffArray(const std::vector<uint8_t>& in_buf,
                   std::vector<uint8_t>& out_buf);
private:

   std::vector<uint8_t>* m_incomplete_msg_ptr;

   //! Pointer that holds the internal state of the unstuffing
   BytestuffingState* m_current_state_ptr;
};

#endif // BYTESTUFFING_FILTER_H_
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
