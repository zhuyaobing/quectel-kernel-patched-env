#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cslclapi" for configuration ""
set_property(TARGET cslclapi APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(cslclapi PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "sreproto;mosquitto;mosquittopp;uuid;systemd;boost_system;boost_filesystem"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcslclapi.so.1"
  IMPORTED_SONAME_NOCONFIG "libcslclapi.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS cslclapi )
list(APPEND _IMPORT_CHECK_FILES_FOR_cslclapi "${_IMPORT_PREFIX}/lib/libcslclapi.so.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
