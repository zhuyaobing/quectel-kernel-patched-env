#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sreproto" for configuration ""
set_property(TARGET sreproto APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(sreproto PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "protobuf"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libsreproto.so.1"
  IMPORTED_SONAME_NOCONFIG "libsreproto.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS sreproto )
list(APPEND _IMPORT_CHECK_FILES_FOR_sreproto "${_IMPORT_PREFIX}/lib/libsreproto.so.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
