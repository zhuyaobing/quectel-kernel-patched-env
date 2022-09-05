/*
***************************************************************************************
***************************************************************************************
***
***     File: FunctorFactory.h
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

   \Subject    Linux API interface definition.

   \Reference  TBD

   \Abstract


   This header file contains class prototypes, data types, functions and other
   definitions required to use the apilib.


   \Version Id:


   "$Id: TransactionFunctorFactory.h 1318 2014-04-04 13:12:29Z STONERIDGE\dahi $"


@{
***************************************************************************************
*/

#ifndef TRANSACTIONFUNCTORFACTORY_H_
#define TRANSACTIONFUNCTORFACTORY_H_

#include <string>
#include <map>
#include <stdint.h>

/*
**=====================================================================================
** Forward declarations, we are only using pointers to the below types
**=====================================================================================
*/
namespace freja
{

class TransactionFunctor;

class TransactionFunctorFactory
{
public:
   TransactionFunctorFactory();
   virtual ~TransactionFunctorFactory();
   bool registerFunctor(std::string functor_name_id, TransactionFunctor *functor_ptr);
   // Returns true if the functor_name_id already exists in the lookup table AND
   // does not replace it. Use remove() to remove the old value.

//      remove();

   TransactionFunctor * create(uint32_t functor_id);

//      find();
   uint32_t getFunctorId(std::string path_function_name);
   static std::string createObjectPathInterfaceFuncName(std::string object_path_name, TransactionFunctor *functor_ptr);


   uint32_t getNextFunctorId();

   const std::string getInterfaceName();


private:
   std::map<std::string, uint32_t > m_string_lookup_table;
   std::map<uint32_t, TransactionFunctor * > m_id_lookup_table;

   uint32_t m_functor_id;
};

} // namespace freja

#endif /* TRANSACTIONFUNCTORFACTORY_H_ */
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
