/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: version.h
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
#ifndef VERSION_H
#define VERSION_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */

#include <string>
#include <cstdint>

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */
namespace ecu {
namespace lapi {

/**
 *******************************************************************************
 *
 * @brief Supporting class for getting LAPI version information
 *
 * @details All access functions are static, hence LapiVersion sould only
 * be used via the static functions - constructor has been deleted.
 *
 * All version information will be set during the LAPI compilation step.
 *
 * The semantic version, SemVer,  will be derived from the TAG which is extract
 * from the git repository.
 *
 *******************************************************************************
 */
class LapiVersion
{
public:
   /**
    ****************************************************************************
    *
    * @brief Abstraction of a semantic version
    *
    ****************************************************************************
    */
   struct SemVer
   {
      /**
       *************************************************************************
       *
       * @brief Represents a version number used in SemVer
       *
       *************************************************************************
       */
      typedef uint8_t Number;
      Number major;  ///< The major version
      Number minor;  ///< The minor version
      Number patch;  ///< The patch/fix version
   };


   /**
    ****************************************************************************
    *
    * @brief Represents a compiler configuration
    *
    ****************************************************************************
    */
   enum CompilerConfiguration
   {
      DEBUG,      ///< debug compiler configuration
      RELEASE,    ///< release compiler configuration
      UKNOWN,     ///< compiler configuration unknown
   };

   /**
    ****************************************************************************
    *
    * @brief Get a string representation of a CompilerConfiguration
    *
    * @param config The CompilerConfiguration of which the function will return
    * the c-string representation.
    *
    * @return The c-string of the provided CompilerConfiguration
    *
    ****************************************************************************
    */
   static const char* configuration_c_str(CompilerConfiguration config) noexcept;

   /**
    ****************************************************************************
    *
    * @brief Get the semantic version for LAPI
    *
    * @return An instance of SemVer which contains the Major, Minor and patch
    * level of LAPI
    *
    ****************************************************************************
    */
   static const SemVer& version() noexcept;

   /**
    ****************************************************************************
    *
    * @brief Get the branch on which LAPI was compiled on
    *
    * @return Return the branch LAPI was compiled on
    *
    ****************************************************************************
    */
   static const char* branch() noexcept;

   /**
    ****************************************************************************
    *
    * @brief The CompilerConfiguration which was used during compilation
    *
    * @return Return an instance of CompilerConfiguration
    *
    ****************************************************************************
    */
   static CompilerConfiguration compiler_config() noexcept;

   /**
    ****************************************************************************
    *
    * @brief The git commit which was used for the LAPI compilation
    *
    * @return The commit hash as a c-string
    *
    ****************************************************************************
    */
   static const char* commit() noexcept;

   /**
    ****************************************************************************
    *
    * @brief The git tag which the LAPI compilation is based on
    *
    * @return The git tag
    *
    ****************************************************************************
    */
   static const char* tag() noexcept;

   /**
    ****************************************************************************
    *
    * @brief The date of git commit
    *
    * @return The commit date as c-string
    *
    ****************************************************************************
    */
   static const char* date() noexcept;

   /**
    ****************************************************************************
    *
    * @brief The meta information of the LAPI configration
    *
    * @details This combines all other information into a single string,
    * similar to a git describe output
    *
    * @return The meta information as a c-string
    *
    ****************************************************************************
    */
   static const char* meta_info() noexcept;


   /**
    ****************************************************************************
    *
    * @brief constructor deleted to disallow instantiation
    *
    ****************************************************************************
    */
   LapiVersion() = delete;

   /**
    ****************************************************************************
    *
    * @brief Copy constructor deleted to disallow copying
    *
    ****************************************************************************
    */
   LapiVersion(const LapiVersion&) = delete;

   /**
    ****************************************************************************
    *
    * @brief Assignment operator deleted to disallow assignments
    *
    ****************************************************************************
    */
   LapiVersion operator=(const LapiVersion&) = delete;
protected:
private:
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

#endif /* VERSION_H */
