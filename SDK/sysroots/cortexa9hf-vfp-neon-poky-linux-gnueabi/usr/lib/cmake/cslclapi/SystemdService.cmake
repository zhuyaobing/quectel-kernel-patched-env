#md
# \section sdcmake SystemdService.cmake
#
# Include this module to automatically generate a service for an application.
#
# \subsection sdcmakeexecuser Executing user
#
# When application service file is generated with this cmake module it will be
# started as the user __rio__ which restricts access to system resources which
# require administrator (root) rights.
#
# If an application requires root privileges (execution as `root`), the boolean
# parameter `EXEC_AS_ROOT` has to be specified.
#
# The parameters `EXEC_AS_ROOT` and `CONTAINER` can however not be combined as
# execution in a container always leads to a private user which is handled by
# systemd (see @ref sdcmakecont).
#
# \subsection sdcmakecont Execution in a container
#
# Setting the parameter `CONTAINER` to `TRUE` will execute the given application
# within an systemd-nspawn container (see `systemd-nspawn(1)` for details).
#
# An container in the alexandra context is an isolated execution environment for
# the application. As such the application will be executed with a container
# specific UID and GID (see nspawn `--private-users`) and an isolated
# filesystem.
#
# Each container will **not** spin up a new init process, instead it will be
# started in a shell-environment which does not require a complete rootfs.
# However, in order to allow an application to be executed, all required
# libraries should be made available in the container. Therefore the
# `alex-container` application - which is called from a systemd service - has
# defined a default set of libraries which are automatically be bind-ro mounted.
#
# * `libsreproto`
# * `libcslclapi`
# * `libstdc++`
# * `libmosquittopp`
# * `libmosquitto`
# * `libprotobuf`
# * `liblzma`
# * `libssl`
# * `libcares`
#
# Additionally libraries can be added using the `CONTAINER_LIB` parameter of
# `add_systemd_service`.
#
#
# Further some default files and directories will be made available within
# the container:
#
# * `/lib`
# * `/etc/hosts`
# * `/etc/nsswitch.conf`
# * `/etc/alex/libcslclapi_rtsupport.so`
# * `/dev/p4_time`
#
# When an app is running in a container, an app-data directory will always be 
# created, which will be available in /data within the container 
# The following bind mount mapping is used
# Host: /common/appdata/[appname]  -> container: /data
#
# Additional files/directories can be added using `CONTAINER_BIND` and/or
# `CONTAINER_BIND_RO` for readonly access.
#endmd

get_filename_component(_module_name_ ${CMAKE_CURRENT_LIST_FILE} NAME)
set(_build_directory_ ${CMAKE_CURRENT_BINARY_DIR})
set(_current_directory_ ${CMAKE_CURRENT_LIST_DIR})

include(CMakeParseArguments)

function(_join VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

function(_prefix_if_not_empty VALUE PREFIX NAME)
   #message("PREFIX: ${NAME} '${VALUE}' '${PREFIX}'")
   if(NOT "${VALUE}" STREQUAL "")
      set(${NAME} "${PREFIX} ${VALUE}" PARENT_SCOPE)
   endif()
endfunction()

function(_required NAME)
   if(NOT ${NAME})
      string(FIND ${NAME} "_" pos)
      math(EXPR pos "${pos}+1")
      string(SUBSTRING ${NAME} ${pos} -1 var_name)
      message(FATAL_ERROR "${_module_name_}: ${var_name} has not been set!")
   endif()
endfunction()

function(_optional NAME DEFAULT_VALUE)
   if(NOT ${NAME})
      string(FIND ${NAME} "_" pos)
      math(EXPR pos "${pos}+1")
      string(SUBSTRING ${NAME} ${pos} -1 var_name)
      message(STATUS "${_module_name_}: ${var_name} has not been set, assume default value '${DEFAULT_VALUE}'")
      set(${NAME} ${DEFAULT_VALUE} PARENT_SCOPE)
   endif()
endfunction()

#md
# \subsection sdcmakefadd Function: add_systemd_service
#
# Use this function to generate a service file and install it on the target.
# This function automaticly export the variable `SYSTEMD_SERVICE_NAME` to the
# parent scope.
#
# This function can be executed multiple times, in case multiple services are
# required. However, the output variable `SYSTEMD_SERVICE_NAME` will be updated
# for each call.
#
# For all parameter describing a service, see systemd.service(5) and
# systemd.unit(5).
#
# For resource related configuration, consult systemd.resource-control(5)
#
# __NOTE: This function does not make any semantic checks of the configuration,
# hence verification of the service configuration needs to be handled manually__
#
# Futher, the function will automaticlly enalbes the service installation using
# cmakes `install` function to `[PREFIX]/lib/systemd/system`.
#
# \subsubsection sdcmakefaddin Input Parameters
#
# * `NAME`: The service name which should be used as a systemd service
#    * __mandatory__
#    * string
# * `UNIT_DESCRIPTION`: Description of the systemd unit
#    * __mandatory__
#    * string
# * `SERVICE_EXEC`: The command which shall be executed by the service
#    * __mandatory__
#    * string
#    * use full path, e.g. `/usr/bin/foobar.exe --param1`
# * `UNIT_AFTER`: After dependency for the unit
#    * __optional__
#    * string
#    * default: `alex-core.target`
# * `UNIT_REQUIRES`: Requires dependency for the unit
#    * __optional__
#    * string
#    * default: `alex-core.target`
# * `UNIT_OTHER`: Other unit section configurations which are not predefined
#    * __optional__
#    * list of strings
#    * Example: `UNIT_OTHER "param1=value1" "param2=value2"`
# * `SERVICE_TYPE`: The type of the systemd service
#    * __optional__
#    * string
#    * default: `simple`
# * `SERVICE_RESTART`: The restart behaviour of the service
#    * __optional__
#    * string
#    * default: `on-failure`
# * `SERVICE_OTHER`: Other service section configurations which are not predefined
#    * __optional__
#    * list of strings
#    * Example: `SERVICE_OTHER "param1=value1" "param2=value2"`
# * `INSTALL_WANTEDBY`: The install configuration for the service
#    * __optional__
#    * string
#    * default: `alex.target`
# * `INSTALL_OTHER`: Other install section configurations which are not predefined
#    * __optional__
#    * list of strings
#    * Example: `INSTALL_OTHER "param1=value1" "param2=value2"`
# * `RESOURCECTL`: Resource control related configuration entries
#    * __optional__
#    * list of strings
#    * Example: `RESOURCECTL "param1=value1" "param2=value2"`
#    * see `systemd.resource-control(5)`
# * `EXEC_AS_ROOT`: Option indicating if the app should be executed as root user
#    * __optional__
#    * bool
#    * this will adjust the generated service file accordingly, remove the
#      `User=rio` directive.
#    * Cannot be combined with `CONTAINER` flag
# * `CONTAINER`: Option indicating if the app should run as container
#    * __optional__
#    * bool
#    * this will imply a different start-up command using `alex-container` which
#      uses `systemd-nspawn`
#    * This needs to be enabled in order for `CONTAINER_*` variables to have
#      effect.
#    * Cannot be combined with `EXEC_AS_ROOT` flag
# * `CONTAINER_MACHINE_NAME`: Override the machine name which is used for the
#   container
#    * __optional__
#    * string
#    * default: basename of application, e.g. `ipcd`
# * `CONTAINER_LIB`: Provide additional libs which shall bound to the container
#    * __optional__
#    * list of strings
#    * default: all LAPI related libraries will be bound to container
#    * Libs required by LAPI will always be included, even when `CONTAINER_LIB`
#      is defined/overridden (@ref sdcmakecont).
#    * Those libs should be specified in its full name e.g. `libcurl.so.4.4.0`
#      with either the absolute path (e.g. `/usr/lib/libcurl.so.4.4.0`) or
#      relative to `/usr/lib`
#    * These libraries will always be bound readonly
# * `CONTAINER_BIND`: Provide additional files or directories which should be
#   made available in the container (from the rootfs) as **writeable**
#    * __optional__
#    * list of strings
#    * default: empty
#    * Entries should be specified with its absolute path and will result in a
#      `--bind` parameter to systemd-nspawn, see systemd-nspawn(1)
# * `CONTAINER_BIND_RO`: Provide additional files or directories which should be
#   made available in the container (from the rootfs) as **readonly**
#    * __optional__
#    * list of strings
#    * default: empty
#    * Entries should be specified with its absolute path and will result in a
#      `--bind-ro` parameter to systemd-nspawn, see systemd-nspawn(1)
# * `CONTAINER_NSPAWN_PARAM`: Provide additional systemd-nspawn parameters to
#   container execution, see systemd-nspawn(1) for more default
#    * __optional__
#    * list of strings
#    * default: empty
#    * Complete nspawn parameters should be specified, e.g. '"--chdir=/my/dir"'.
#      Note that quotes need to be provided!
#
# \subsubsection sdcmakefaddout Output Parameters
#
# * `SYSTEMD_SERVICE_NAME`:
#   A string containing the generated service name `[name].service`
#
# \subsubsection sdcmakefaddex Example
#
# ~~~{cmake}
# ################################################################################
# # Systemd Service file creation
# ################################################################################
#
# include(SystemdService)
#
# add_systemd_service(
#   NAME foobar
#   UNIT_DESCRIPTION "This is the Description"
#   SERVICE_EXEC "/usr/bin/foobar --config foobar.config"
#   RESOURCECTL
#     "CPUAccounting=true"
#     "MemoryAccounting=true"
# )
#
# # assigned exported variable to a local variable to be used later
# set(foobar_service_name ${SYSTEMD_SERVICE_NAME})
#
# # use service variable to enable the service via the package generation
# set(CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE ${foobar_service_name})
#
# # or use the exported variable directly
# #set(CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE ${SYSTEMD_SERVICE_NAME})
# ~~~
#
# The generated service file for this example will look like this:
# ~~~
# [Unit]
# Description=This is the Description
# After=alex-core.target
# Requires=alex-core.target
#
# [Service]
# Type=simple
# Restart=on-failure
# ExecStart=/usr/bin/foobar --config foobar.config
#
# CPUAccounting=true
# MemoryAccounting=true
#
# [Install]
# WantedBy=alex.target
#
# ~~~
#
#
# \subsubsection sdcmakefaddexcont Container Example
#
# The following example will configure a container for the `mycontainer`
# application. It is assumed that the application is linked against `libcurl`
# and `libbz2` which requires that the libraries will be available from within
# the container (see `CONTAINER_LIB`).
#
# ~~~{cmake}
# ################################################################################
# # Systemd Service file executed as a container
# ################################################################################
#
# set(APP_NAME "mycontainer")
#
# include(SystemdService)
#
# add_systemd_service(
#   NAME ${APP_NAME}
#   UNIT_DESCRIPTION "My container application"
#   SERVICE_EXEC "/usr/bin/${APP_NAME}"
#   # container config parameter!
#   CONTAINER TRUE
#   CONTAINER_LIB
#         "libcurl"
#         "libbz2"
# )
#
# # set service name for packaging
# set(CPACK_OPKG_PACKAGE_SYSTEMD_SERVICE ${SYSTEMD_SERVICE_NAME})
# ~~~
#
#endmd
function(add_systemd_service)
   set(options
      CONTAINER
      EXEC_AS_ROOT
   )
   set(oneValueArgs
      UNIT_DESCRIPTION
      UNIT_AFTER
      UNIT_REQUIRES
      NAME
      SERVICE_EXEC
      SERVICE_TYPE
      SERVICE_RESTART
      INSTALL_WANTEDBY
      CONTAINER_MACHINE_NAME
   )
   set(multiValueArgs
      RESOURCECTL
      UNIT_OTHER
      SERVICE_OTHER
      INSTALL_OTHER
      CONTAINER_LIB
      CONTAINER_BIND
      CONTAINER_BIND_RO
      CONTAINER_NSPAWN_PARAM
   )
   cmake_parse_arguments(
      S "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
   )

   # note: all variables that are prefixed with 'S_' are steered by the user(
   # function call. Variables starting with '_' are considered private to the
   # function (e.g '_S_EXEC)

   _required(S_NAME)
   _required(S_UNIT_DESCRIPTION)
   _required(S_SERVICE_EXEC)

   _optional(S_UNIT_AFTER "alex-core.target")
   _optional(S_UNIT_REQUIRES "alex-core.target")
   _optional(S_SERVICE_RESTART "on-failure")
   _optional(S_SERVICE_TYPE "simple")
   _optional(S_INSTALL_WANTEDBY "alex.target")

   _join("${S_RESOURCECTL}" "\n" S_RESOURCECTL)
   _join("${S_UNIT_OTHER}" "\n" S_UNIT_OTHER)
   _join("${S_SERVICE_OTHER}" "\n" S_SERVICE_OTHER)
   _join("${S_INSTALL_OTHER}" "\n" S_INSTALL_OTHER)

   if(S_CONTAINER AND S_EXEC_AS_ROOT)
      message(FATAL_ERROR "CONTAINER and EXEC_AS_ROOT can't be combined, select either or none!")
   endif()

   if(S_CONTAINER)
      _optional(S_CONTAINER_MACHINE_NAME "")
      _prefix_if_not_empty("${S_CONTAINER_MACHINE_NAME}" "-m" S_CONTAINER_MACHINE_NAME)
      _optional(S_CONTAINER_LIB "")
      _optional(S_CONTAINER_BIND "")
      _optional(S_CONTAINER_BIND_RO "")
      _optional(S_CONTAINER_NSPAWN_PARAM "")

      _join("${S_CONTAINER_LIB}" " -l " S_CONTAINER_LIB)
      _prefix_if_not_empty("${S_CONTAINER_LIB}" "-l" S_CONTAINER_LIB)

      _join("${S_CONTAINER_BIND}" " -b " S_CONTAINER_BIND)
      _prefix_if_not_empty("${S_CONTAINER_BIND}" "-b" S_CONTAINER_BIND)

      _join("${S_CONTAINER_BIND_RO}" " -r " S_CONTAINER_BIND_RO)
      _prefix_if_not_empty("${S_CONTAINER_BIND_RO}" "-r" S_CONTAINER_BIND_RO)

      _join("${S_CONTAINER_NSPAWN_PARAM}" " -p " S_CONTAINER_NSPAWN_PARAM)
      _prefix_if_not_empty("${S_CONTAINER_NSPAWN_PARAM}" "-p" S_CONTAINER_NSPAWN_PARAM)

      set(S_CONTAINER_BIND "-b /common/appdata/${APP_NAME}:/data ${S_CONTAINER_BIND}")      

      set(_alex_container_ "/usr/bin/alex-container ${S_CONTAINER_MACHINE_NAME} ${S_CONTAINER_LIB} ${S_CONTAINER_BIND} ${S_CONTAINER_BIND_RO} ${S_CONTAINER_NSPAWN_PARAM}")

      set(S_SERVICE_EXEC "${_alex_container_} ${S_SERVICE_EXEC}")
   else()
      # if exec as root is set, the '_S_EXEC_USER' variable will *not* be set!
      if(NOT S_EXEC_AS_ROOT)
         set(_S_EXEC_USER "User=rio")
      else()
         message(STATUS "${_module_name_}: Application '${S_NAME}' will be enabled for execution as 'root'.")
      endif()
   endif()


   string(TOLOWER ${S_NAME} service_name)
   set(_service_file_ "${_build_directory_}/${service_name}.service")
   message(STATUS "${_module_name_} Generate service file: ${_service_file_}")


   configure_file(
      ${_current_directory_}/in/systemd.service.in
      ${_service_file_}
   )

   set(SYSTEMD_SERVICE_NAME "${service_name}.service" PARENT_SCOPE)

   # install service file
   install(
      FILES ${_service_file_}
      DESTINATION /lib/systemd/system
      COMPONENT init
      )
endfunction()


