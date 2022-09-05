#md
# \section packagecmake SrePackage.cmake
#
# This module shall be used to pack the application or library into an opkg
# package (ipk-file). Therefore all cmake install target configuration should be
# done correctly.
#
# The packaging makes use of cmake-packaging utilizing `cpack`. Unfortunately
# cmake-packaging cannot be extended with a specific package format, therefore
# the `DEBIAN` has been reused. This implies that apart from writing out just
# an ipk-file, a debian `deb` will also be created which, however, can be
# ignored. Only the generated ipk is required for an installation on the target.
#
# \subsection packagecmakeusage Usage
#
# Since cmake-packaging will prepare every on the inclusion of the `CPack`
# cmake module (implicitly included with this module), all variables need to be
# set before the `include` statement.
#
# By default, `sre-lapi` dependency will be added. To disable this behaviour,
# set `CPACK_OPKG_PACKAGE_DEPENDS_NO_LAPI`: `set(CPACK_OPKG_PACKAGE_DEPENDS_NO_LAPI True)`
#
# __NOTE__: For systemd-service installation and control file extensions, see
# 'Special variables' section below.
#
#
# \subsection packagecmakeconfig OKPG Package configuration
#
# In order to configure the package to be generated, mandatory variables,
# starting with: `CPACK_OPKG_PACAKGE_*` should be set.
#
# \subsubsection packagecmakemanvar Mandatory variables
#
# * `CPACK_OPKG_PACKAGE_DESCRIPTION`: A description of the package
#    * __mandatory__
#    * string
#    * default: `CPACK_PACKAGE_DESCRIPTION_SUMMARY`
# * `CPACK_OPKG_PACKAGE_MAINTAINER`: Maintainer of the pacakge
#    * __mandatory__
#    * string
#    * format: `foo bar <foo.bar@baz.com>`
#    * default: `CPACK_PACKAGE_CONTACT`
#
# \subsubsection packagecmakeoptvar Optional variables
#
# * `CPACK_OPKG_PACKAGE_FILE_NAME`:  The name of the package file to generate,
# not including the file-extension.
#    * __optional__
#    * string
#    * Example: `cmake-2.6.1-Linux-i686`
#    * default: `${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}`,
#      will be extracted from the cmake PROJECT name set
# * `CPACK_OPKG_PACKAGE_NAME`:  The name of the package
#    * __optional__
#    * string
#    * default: to `CPACK_PACKAGE_NAME` which is by default set to PROJECT_NAME
# * `CPACK_OPKG_PACKAGE_VERSION`: Package version, full version (x.x.x)
#    * __optional__
#    * string
#    * default: `MAJOR.MINOR.PATCH` - will use `CPACK_PACKAGE_VERSION_MAJOR`, `CPACK_PACKAGE_VERSION_MINOR`, and
# `CPACK_PACKAGE_VERSION_PATCH`
# * `CPACK_OPKG_PACKAGE_DEPENDS`: Specifies the package dependencies
#    * __optional__
#    * space separated list of strings
#    * default: `""`
# * `CPACK_OPKG_PACKAGE_DEPENDS_NO_LAPI`: Disable automaitc lapi dependency
#    * __optional__
#    * boolean
#    * default: undefined (equals false)
# * `CPACK_OPKG_PACKAGE_LICENSE`: The license of the package
#    * __optional__
#    * string
#    * default: `""`
# * `CPACK_OPKG_PACKAGE_SECTION`: Section in which the package should be sorted in
#    * __optional__
#    * string
#    * default: `extras`
# * `CPACK_OPKG_PACKAGE_PRIORITY`:  The priority of the package
#    * __optional__
#    * string
#    * default: `optional`
# * `CPACK_OPKG_PACKAGE_SOURCE`:
#    * __optional__
#    * string
#    * default: `None`
#    * example: `https://www.stoneridgeelectronics.com`
# * `CPACK_OPKG_PACKAGE_PROVIDES`:
#    * __optional__
#    * space separated list of string
#    * default: `""`
# * `CPACK_OPKG_PACKAGE_REPLACES`:
#    * __optional__
#    * space separated list of string
#    * default: `""`
# * `CPACK_OPKG_PACKAGE_CONFLICTS`:
#    * __optional__
#    * space separated list of string
#    * default: `""`
#
#
# \subsubsection packagecmakespecvar Special variables
#
# * `CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE`: Specifies `systemd`-services that should be enabled during installation
#    * __optional__
#    * space separated list of strings
#    * Internally, this variable controls if default `systemctl` `enable` and `disable` should be called
#      during installation and deinstallation, respectively.
#    * The services always need to be specified with `.service`
#    * example: `foobar.service`
#
# * `CPACK_OPKG_PACKAGE_<TYPE>_FILE`: Script content that shall be add to the specified file type.
#    * `<TYPE>` should be:
#        * PREINST
#        * PRERM
#        * POSTINST
#        * POSTRM
#    * __optional__
#    * In case of `POSTINST` and `PRERM` and systemd service(s) is specified, the script will be appended
#    * see below for an elaborate example
#
# * `CPACK_OPKG_PACKAGE_ARCHITECTURE`: target architecture
#    * Is mandatory but will be generated internally, the user does not need
#      to set it!
#
# * `CPACK_OPKG_PACKAGE_USE_GIT_VERSION`: Use `git describe` to extract the package
#    version.
#    * __optional__
#    * boolean: True, False or don't set
#    * This will have precedence over setting `CPACK_OPKG_PACKAGE_VERSION`
#
# \subsection packagecmakeex Example
#
# ~~~{cmake}
# ################################################################################
# # PACKAGING HANDLING
# ################################################################################
#
# set(CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE foobar.service)
#
# set(CPACK_OPKG_PACKAGE_MAINTAINER "John Doe <john.doe@stoneridge.com>")
# include(SrePackage)
# ################################################################################
# ~~~
#
# Use `make package` within the build directory to build the ipk-file. The
# ipk-file will be avaiable in the top-level of the build-directory.
#
#
# \subsection packagecmakectlf OKPG control file extensions
#
# Set `CPACK_OPKG_PACKAGE_(PRE|POST)(INST|RM))_FILE` variables to extend the
# corresponding ipk-file control files.
#
# ~~~{cmake}
# # extend the POSTINST file opkg control file, assuming also that myapp.service
# # should be enabled
#
# # enable myapp.service
# set(CPACK_OPKG_PACKAGE_POSTINST_FILE "echo 'foobar'
# echo 'loglevel=debug' >> /etc/myapp/myapp.conf
# ")
# ~~~
#
# This will result in `postinst` in the ipk-file:
#
# ~~~{shell}
#!/bin/sh
# OPTS=""
#
# if [ -n "$D" ]; then
#     OPTS="--root=$D"
# fi
#
#
# if type systemctl >/dev/null 2>/dev/null; then
#    systemctl $OPTS enable myapp.service
# fi
#
#
# ################################################################################
# ### START INPUT from user-provided file
# ################################################################################
# echo 'foobar'
# echo 'loglevel=debug' >> /etc/myapp/myapp.conf
#
# ################################################################################
# ### END INPUT
# ################################################################################
#
# if [ x"$D" = "x" ]; then
# 	if [ -x /sbin/ldconfig ]; then /sbin/ldconfig ; fi
# fi
# ~~~
#
#endmd

get_filename_component(_module_name_ ${CMAKE_CURRENT_LIST_FILE} NAME)

include (TargetArch)
target_architecture(DETECTED_ARCH)

# Get target package arch from Yocto ADT sysroot if set or host OS, mapping to Ubuntu name if necessary
if (DEFINED ENV{OECORE_TARGET_SYSROOT})
   GET_FILENAME_COMPONENT (DETECTED_SYSROOT $ENV{OECORE_TARGET_SYSROOT} NAME)
   string (REGEX REPLACE "-poky-linux-gnueabi" "" TARGET_ARCH "${DETECTED_SYSROOT}")
else ()
   # Debian uses amd64 to denote x86_64
   if (DETECTED_ARCH STREQUAL "x86_64")
      set (TARGET_ARCH "amd64")
   else ()
      set (TARGET_ARCH ${DETECTED_ARCH})
   endif ()
endif ()
set(CPACK_OPKG_PACKAGE_ARCHITECTURE ${TARGET_ARCH})
message(STATUS "${_module_name_}: Package arch is ${CPACK_OPKG_PACKAGE_ARCHITECTURE}")

set(CPACK_GENERATOR "DEB")
set(CPACK_BINARY_DIR "${CMAKE_BINARY_DIR}")

if(CPACK_OPKG_PACKAGE_USE_GIT_VERSION)
   execute_process(
      COMMAND git describe
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE GIT_TAG
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )
   set(CPACK_OPKG_PACKAGE_VERSION ${GIT_TAG})
   message(STATUS "${_module_name_}: Use extracted git version: '${CPACK_OPKG_PACKAGE_VERSION}'")
endif()

include(CPack)
