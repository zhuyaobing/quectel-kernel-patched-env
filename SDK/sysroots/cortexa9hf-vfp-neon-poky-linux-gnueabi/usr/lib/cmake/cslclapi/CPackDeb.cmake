get_filename_component(_module_name_ ${CMAKE_CURRENT_LIST_FILE} NAME)

################################################################################

if(CMAKE_BINARY_DIR)
   message(FATAL_ERROR "${_module_name_} may only be used by CPack internally.")
endif()

if(NOT UNIX)
   message(FATAL_ERROR "${_module_name_} may only be used under UNIX.")
endif()

find_program(OPKG_CMD opkg-build)
if(NOT OPKG_CMD)
   message(FATAL_ERROR "${_module_name_}: opkg-build not found!")
endif()

################################################################################

# Description: (mandatory)
if(NOT CPACK_OPKG_PACKAGE_DESCRIPTION)
   if(NOT CPACK_PACKAGE_DESCRIPTION_SUMMARY)
      message(FATAL_ERROR "${_module_name_}: OPKG package requires a summary for a package, set CPACK_PACKAGE_DESCRIPTION_SUMMARY or CPACK_OPKG_PACKAGE_DESCRIPTION")
   endif()
   set(CPACK_OPKG_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_OPKG_PACKAGE_DESCRIPTION})

# Maintainer: (mandatory)
if(NOT CPACK_OPKG_PACKAGE_MAINTAINER)
   if(NOT CPACK_PACKAGE_CONTACT)
      message(FATAL_ERROR "${_module_name_}: OPKG package requires a maintainer for a package, set CPACK_PACKAGE_CONTACT or CPACK_OPKG_PACKAGE_MAINTAINER")
   endif()
   set(CPACK_OPKG_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_OPKG_PACKAGE_MAINTAINER})

# Architecture: (mandatory)
# Should be set by module that includes this module, e.g. SrePackage.cmake
if(NOT CPACK_OPKG_PACKAGE_ARCHITECTURE)
   message(FATAL_ERROR "${_module_name_}: OPKG package requires an architecture, set CPACK_OPKG_PACKAGE_ARCHITECTURE.")
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_OPKG_PACKAGE_ARCHITECTURE})


# Package file name: (optional)
# The name of the package file to generate, not including the extension. For
# example, cmake-2.6.1-Linux-i686. The default value is:
# ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}.
if(NOT CPACK_OPKG_PACKAGE_FILE_NAME)
   set(CPACK_OPKG_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_FILE_NAME ${CPACK_OPKG_PACKAGE_FILE_NAME})

# Packag name: (optional)
# will default to CPACK_PACKAGE_NAME which is by default set to
# PROJECT_NAME
if(NOT CPACK_OPKG_PACKAGE_NAME)
   set(CPACK_OPKG_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
endif()
string(TOLOWER "${CPACK_OPKG_PACKAGE_NAME}" CPACK_OPKG_PACKAGE_NAME)
string(REPLACE "_" "-" CPACK_OPKG_PACKAGE_NAME "${CPACK_OPKG_PACKAGE_NAME}")
set(GEN_CPACK_DEBIAN_PACKAGE_NAME ${CPACK_OPKG_PACKAGE_NAME})

# Package version: (optional)
# Package full version, used internally. By default, this is built from
# CPACK_PACKAGE_VERSION_MAJOR, CPACK_PACKAGE_VERSION_MINOR, and
# CPACK_PACKAGE_VERSION_PATCH.
if(NOT CPACK_OPKG_PACKAGE_VERSION)
   if(NOT CPACK_PACKAGE_VERSION)
      message(FATAL_ERROR "${_module_name_}: OPKG package requires package version.")
   endif()
   set(CPACK_OPKG_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_OPKG_PACKAGE_VERSION})

# Dependencies: (optional)
# Specifies the package dependencies
if(NOT CPACK_OPKG_PACKAGE_DEPENDS)
   message("${_module_name_}: CPACK_OPKG_PACKAGE_DEPENDS not set, the package will have no dependencies.")
endif()
if(NOT CPACK_OPKG_PACKAGE_DEPENDS_NO_LAPI)

   message("${_module_name_}: default LAPI dependency added 13.3.0-2-g7c58342")
   if(NOT CPACK_OPKG_PACKAGE_DEPENDS)
      set(CPACK_OPKG_PACKAGE_DEPENDS "sre-lapi (>= 13.3.0-2-g7c58342)")
   else()
      set(CPACK_OPKG_PACKAGE_DEPENDS "sre-lapi (>= 13.3.0-2-g7c58342), ${CPACK_OPKG_PACKAGE_DEPENDS}")
   endif()
endif()


# Section: (optional)
# defaults to "extras"
if(NOT CPACK_OPKG_PACKAGE_SECTION)
   set(CPACK_OPKG_PACKAGE_SECTION "extras")
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_SECTION ${CPACK_OPKG_PACKAGE_SECTION})

# Priority: (optional)
# defaults to "optional"
if(NOT CPACK_OPKG_PACKAGE_PRIORITY)
   set(CPACK_OPKG_PACKAGE_PRIORITY "optional")
endif()
set(GEN_CPACK_DEBIAN_PACKAGE_PRIORITY ${CPACK_OPKG_PACKAGE_PRIORITY})

# Source: (optional)
# the source of the package, defaults to 'None'
if(NOT CPACK_OPKG_PACKAGE_SOURCE)
   set(CPACK_OPKG_PACKAGE_SOURCE "None")
endif()

# License: (optional)
# the license of the package, defaults to 'CLOSED'
if(NOT CPACK_OPKG_PACKAGE_LICENSE)
   set(CPACK_OPKG_PACKAGE_LICENSE "CLOSED")
endif()

# Source: (optional)
# CPACK_OPKG_PACKAGE_PROVIDES

# Replaces: (optional)
# CPACK_OPKG_PACKAGE_REPLACES

# Conflicts: (optional)
# CPACK_OPKG_PACKAGE_CONFLICTS

# systemd service: (optional)
# Single value or list to enable systemd service during installation
# CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE

#CPACK_OPKG_PACKAGE_PREINST_FILE
#CPACK_OPKG_PACKAGE_PRERM_FILE
#CPACK_OPKG_PACKAGE_POSTRM_FILE
#CPACK_OPKG_PACKAGE_POSTINST_FILE

if(CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE)
   foreach(_service ${CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE})
      string(REGEX MATCH "\\.service$" ends_with_service ${_service})

      if("${ends_with_service}" STREQUAL "")
         message(FATAL_ERROR "${_module_name_}: Entries of CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE need to end with '.service', current value: '${CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE}'")
      endif()

      message("${_module_name_}: Enable/disable systemd service script for ${_service}")

      set(ENABLE_SYSTEMD_SERVICE "${ENABLE_SYSTEMD_SERVICE}
if type systemctl >/dev/null 2>/dev/null; then
   systemctl $OPTS enable ${_service}

   if [ -z \"$D\" ]; then
      systemctl restart ${_service}
   fi
fi
")

      set(DISABLE_SYSTEMD_SERVICE "${DISABLE_SYSTEMD_SERVICE}
if type systemctl >/dev/null 2>/dev/null; then
   if [ -z \"$D\" ]; then
      systemctl stop ${_service}
   fi

   systemctl $OPTS disable ${_service}
fi
")

   endforeach()
endif()

################################################################################
# Generate opkg package, ipk-file, according to variables set!
################################################################################


set(CPACK_OPKG_ROOTDIR "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_OPKG_PACKAGE_FILE_NAME}")
set(CPACK_OPKG_CONTROL_DIR "${CPACK_OPKG_ROOTDIR}/CONTROL")
file(MAKE_DIRECTORY ${CPACK_OPKG_CONTROL_DIR})
set(CPACK_OPKG_CONTROL_FILE "${CPACK_OPKG_CONTROL_DIR}/control")

set(_current_directory_ ${CMAKE_CURRENT_LIST_DIR})
# contains systemd directive
configure_file(
   ${_current_directory_}/in/ipk.postinst.in
   ${CPACK_OPKG_CONTROL_DIR}/postinst
   @ONLY)
# contains systemd directive
configure_file(
   ${_current_directory_}/in/ipk.prerm.in
   ${CPACK_OPKG_CONTROL_DIR}/prerm
   @ONLY)

configure_file(
   ${_current_directory_}/in/ipk.postrm.in
   ${CPACK_OPKG_CONTROL_DIR}/postrm
   @ONLY)
configure_file(
   ${_current_directory_}/in/ipk.preinst.in
   ${CPACK_OPKG_CONTROL_DIR}/preinst
   @ONLY)

# Write controlfile
file(WRITE ${CPACK_OPKG_CONTROL_FILE}
"Package: ${CPACK_OPKG_PACKAGE_NAME}
Version: ${CPACK_OPKG_PACKAGE_VERSION}
Description: ${CPACK_OPKG_PACKAGE_DESCRIPTION}
Architecture: ${CPACK_OPKG_PACKAGE_ARCHITECTURE}
Section: ${CPACK_OPKG_PACKAGE_SECTION}
Priority: ${CPACK_OPKG_PACKAGE_PRIORITY}
Maintainer: ${CPACK_OPKG_PACKAGE_MAINTAINER}
Depends: ${CPACK_OPKG_PACKAGE_DEPENDS}
Provides: ${CPACK_OPKG_PACKAGE_PROVIDES}
Replaces: ${CPACK_OPKG_PACKAGE_REPLACES}
Conflicts: ${CPACK_OPKG_PACKAGE_CONFLICTS}
Source: ${CPACK_OPKG_PACKAGE_SOURCE}
License: ${CPACK_OPKG_PACKAGE_LICENSE}
")



set(OPKG_FILE_NAME "${CPACK_OPKG_PACKAGE_NAME}_${CPACK_OPKG_PACKAGE_VERSION}_${CPACK_OPKG_PACKAGE_ARCHITECTURE}")
execute_process(
   COMMAND "${OPKG_CMD}" "-o" "0" "${CPACK_OPKG_PACKAGE_FILE_NAME}" "."
   RESULT_VARIABLE _result
   OUTPUT_VARIABLE _res_output
   ERROR_VARIABLE  _res_error
   WORKING_DIRECTORY ${CPACK_TOPLEVEL_DIRECTORY}
   )

if(${_result})
   message("Result '${_result}'")
   message("Output '${_res_output}'")
   message("Error  '${_res_error}'")
else(${_result})
   message("${_module_name_}: Package ${OPKG_FILE_NAME}.ipk generated.")
   #set(WDIR "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_OPKG_PACKAGE_FILE_NAME}")
   file(RENAME ${CPACK_TOPLEVEL_DIRECTORY}/${OPKG_FILE_NAME}.ipk ${CPACK_BINARY_DIR}/${OPKG_FILE_NAME}.ipk)
endif(${_result})

################################################################################
# Preparation for DEB packaging:
#
# Required to *not* let the deb generation fail, hence the cpack execution to
# segfault.
################################################################################
set(WDIR "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_OPKG_PACKAGE_FILE_NAME}")
set(GEN_WDIR "${WDIR}")
set(GEN_CPACK_DEBIAN_COMPRESSION_TYPE "gzip")

find_program(FAKEROOT_EXECUTABLE fakeroot)
if(NOT FAKEROOT_EXECUTABLE)
   message(FATAL_ERROR "${_module_name_}: Unable to find fakeroot executable")
endif()
set(GEN_CPACK_DEBIAN_FAKEROOT_EXECUTABLE ${FAKEROOT_EXECUTABLE})
