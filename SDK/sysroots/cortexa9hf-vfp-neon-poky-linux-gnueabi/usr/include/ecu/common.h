/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: common.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
 ***
 *******************************************************************************
 *******************************************************************************
 ***
 ***
 ***   Subject:
 ***
 ***   Document reference:
 ***
 ***   Description of this header file:
 ***
 ***  Version:
 ***
 ***
 *******************************************************************************
 */


/*
 **=============================================================================
 ** Header definition
 **=============================================================================
 */
#ifndef COMMON_H
#define COMMON_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */
#include <memory>

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */
namespace ecu {
namespace lapi {

template<class T, class Error = int>
struct Result
{
   Result(bool success) : _success(success), _value() { }
   Result(bool success, Error err) : _success(success), _value(), _err(err) { }
   Result(bool success, T&& t) : _success(success), _value(std::move(t)) { }
   Result(bool success, T&& t, Error err) :
      _success(success), _value(std::move(t)), _err(err) { }
   bool ok() const { return _success; }
   bool nok() const { return !_success; }
   T& val() { return _value; }
   T take_val() { return std::move(_value); }
   const Error& err_val() const { return _err; }
private:
   bool _success;
   T _value;
   Error _err;
};

/*
 **=============================================================================
 ** Public constant definitions for external access
 **=============================================================================
 */

/*
 **=============================================================================
 ** Function prototype declarations for external access
 **=============================================================================
 */
}} /* end namespace */

#endif /* COMMON_H */
