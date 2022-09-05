#md
# \section versiongencmake VersionHdrGen.cmake
#
# This module is Used for generating a version header file based on a git
# repository, using `git describe`.
#
# \subsection versiongencmakusage Usage
#
# Make sure that git is installed when using this module. Further, at least
# one *annotated* tag should be available in the git repository as git-describe
# is requiring in order work correctly.
#
# The version file will be enabled when including this cmake module, no function
# need to be called. This implies that input variables need to be configured be
# the actually `include` statement.
#
# **NOTE**: The generated header-file will be generated in the
# `CMAKE_CURRENT_BINARY_DIR` and automatically added as an include directory to
# cmake using `include_directories`. In case the `include(VersionHdrGen)`
# statement is done in the top-level ĆMakeLists.txt, the header file will be
# located in the top-level of the build-dirctory (e.g. `build/version.h`).
#
# \subsection versiongencmakeconfig Configuration
#
# \subsubsection versiongencmakeinvar Input Variables
#
# * `VERSION_GEN_H_FILE_NAME`: The header file name which should be generated
#    * __optional__
#    * string
#    * specify just the
#    * Default: `version.h`
#    * Example: `set(VERSION_GEN_H_FILE_NAME "gen_version.h")`
#
#
# \subsubsection versiongencmakeoutvar Output Variables
# * `VERSION_GEN_TARGET`: In order to add dependencies to other targets,
#   this modules exports the target `VERSION_GEN_TARGET`.
#    * Example: `add_dependencies(binary-name ${VERSION_GEN_TARGET})`
#
# \subsection versiongencmakehdr Header output
#
# The generated header file sets the following pre-processor defines:
# * `GIT_BRANCH`: The name of the current checked out branch
# * `GIT_COMMIT_HASH`: The current commit in short hash form
# * `GIT_DATE`: Date when current commit was created
# * `GIT_TAG`: `git describe` output
# * `CONF_MODE`: Set the string representation of the cmake build type (`cmake -DCMAKE_BUILD_TYPE=Debug`)
# * `META_INFO`: Combines all previous information into a single string
#
# The resulting heading will look like this:
#
# ~~~{cpp}
# #ifndef GEN_VERSION_H
# #define GEN_VERSION_H
#
# # ifdef __cplusplus
# extern "C" {
# # endif
#
# #define GIT_BRANCH      "test_branch"      /*! Current name of the branch     */
# #define GIT_COMMIT_HASH "7a279af" /*! Current hash of the git branch */
# #define GIT_DATE        "2017-06-16 17:54:25 +0200" /*! Current date of the git branch */
# #define GIT_TAG         "2.4.1-13-g7a279af"         /*! Current tag of the git branch  */
#
# #if NDEBUG
# #define CONF_MODE       "RELEASE"/*! Last compilation mode used */
# #else
# #define CONF_MODE       "DEBUG" /*! Last compilation mode used */
# #endif
#
# #define META_INFO       GIT_BRANCH " - " GIT_TAG " - " GIT_COMMIT_HASH " - " GIT_DATE " - " CONF_MODE /*! Version name */
#
# # ifdef __cplusplus
# }
# # endif
#
# #endif /* GEN_VERSION_H */
# ~~~
#
# \subsection versiongencmakeex Example
#
# ~~~{cmake}
# ################################################################################
# # VERSION HANDLING
# ################################################################################
# set(VERSION_GEN_H_FILE_NAME "gen_version.h")
# include(VersionHdrGen)
# add_dependencies(${MYAPP_NAME} ${VERSION_GEN_TARGET})
# ################################################################################
# ~~~
#
# The header file `gen_version.h` will now be generated during the build step of
# the application and can be included using
# ~~~{cpp}
# #include <gen_version.h>
# ~~~
#
#endmd

get_filename_component(_module_name_ ${CMAKE_CURRENT_LIST_FILE} NAME)

set(_build_directory_ ${CMAKE_CURRENT_BINARY_DIR})
set(_current_directory_ ${CMAKE_CURRENT_LIST_DIR})
set(_version_h_in_ ${_current_directory_}/in/version.h.in)
set(_version_cmake_in_ ${_current_directory_}/in/version.cmake.in)
set(_version_cmake_ ${_build_directory_}/version.cmake)


if(NOT VERSION_GEN_H_FILE_NAME)
   set(VERSION_GEN_H_FILE_NAME version.h)
endif()
set(_version_h_file_ ${_build_directory_}/${VERSION_GEN_H_FILE_NAME})
message(STATUS "${_module_name_}: Generate '${_version_h_file_}'")

set(VERSION_GEN_TARGET "${PROJECT_NAME}_version_hdr_gen")

configure_file(
   ${_version_cmake_in_}
   ${_version_cmake_}
)

include_directories(${_build_directory_})
set_source_files_properties(${_version_h_file_} PROPERTIES GENERATED TRUE)

add_custom_command(OUTPUT always_rebuild COMMAND true COMMENT "Always rebuild")
add_custom_command(
   OUTPUT ${_version_h_file_}
   COMMAND cmake -P ${_version_cmake_}
   DEPENDS
      always_rebuild
      ${_version_cmake_in_}
   COMMENT "Generating ${VERSION_GEN_H_FILE_NAME} for project ${PROJECT_NAME}")

add_custom_target(${VERSION_GEN_TARGET}
   ALL
   DEPENDS ${_version_h_file_})
