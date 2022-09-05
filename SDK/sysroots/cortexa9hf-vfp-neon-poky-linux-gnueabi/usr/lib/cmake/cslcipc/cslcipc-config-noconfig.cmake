#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cslcipc" for configuration ""
set_property(TARGET cslcipc APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(cslcipc PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "sreproto;mosquitto;mosquittopp;uuid;systemd"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcslcipc.so.1"
  IMPORTED_SONAME_NOCONFIG "libcslcipc.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS cslcipc )
list(APPEND _IMPORT_CHECK_FILES_FOR_cslcipc "${_IMPORT_PREFIX}/lib/libcslcipc.so.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
