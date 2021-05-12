# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindSYSTEMD
--------

Find Systemd (SYSTEMD)

Find the systemd libraries (``libsystemd``)

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``SYSTEMD::SYSTEMD``, if
SYSTEMD has been found.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``SYSTEMD_FOUND``
  True if SYSTEMD_INCLUDE_DIR & SYSTEMD_LIBRARY are found

``SYSTEMD_LIBRARIES``
  List of libraries when using SYSTEMD.

``SYSTEMD_INCLUDE_DIRS``
  Where to find the SYSTEMD headers.

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``SYSTEMD_INCLUDE_DIR``
  the SYSTEMD include directory

``SYSTEMD_LIBRARY``
  the absolute path of the systemd library
#]=======================================================================]

find_package(PkgConfig)
pkg_check_modules(PC_SYSTEMD QUIET libsystemd)
find_library(SYSTEMD_LIBRARY NAMES systemd ${PC_SYSTEMD_LIBRARY_DIRS})
find_path(SYSTEMD_INCLUDE_DIR systemd/sd-login.h HINTS ${PC_SYSTEMD_INCLUDE_DIRS})



include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SYSTEMD
                                  REQUIRED_VARS SYSTEMD_LIBRARY SYSTEMD_INCLUDE_DIR
                                  VERSION_VAR SYSTEMD_VERSION_STRING)

if(SYSTEMD_FOUND)
  set( SYSTEMD_LIBRARIES ${SYSTEMD_LIBRARY} )
  set( SYSTEMD_INCLUDE_DIRS ${SYSTEMD_INCLUDE_DIR} )
  if(NOT TARGET SYSTEMD::SYSTEMD)
    add_library(SYSTEMD::SYSTEMD UNKNOWN IMPORTED)
    set_target_properties(SYSTEMD::SYSTEMD PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SYSTEMD_INCLUDE_DIRS}")
    set_property(TARGET SYSTEMD::SYSTEMD APPEND PROPERTY IMPORTED_LOCATION "${SYSTEMD_LIBRARY}")
  endif()
endif()

if(SYSTEMD_FOUND AND "${SYSTEMD_UNITDIR}" STREQUAL "")
  execute_process(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd
    OUTPUT_VARIABLE SYSTEMD_UNITDIR
  )
  string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_UNITDIR "${SYSTEMD_UNITDIR}")
  message(STATUS "SYSTEMD_UNITDIR is ${SYSTEMD_UNITDIR}")
elseif(NOT SYSTEMD_FOUND AND SYSTEMD_UNITDIR)
  message(FATAL_ERROR "Variable SYSTEMD_UNITDIR is\
		defined, but we can't find systemd using pkg-config"
  )
endif()
mark_as_advanced(SYSTEMD_INCLUDE_DIR SYSTEMD_LIBRARY SYSTEMD_UNITDIR)
