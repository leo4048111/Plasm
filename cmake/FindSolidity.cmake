#[=======================================================================[.rst:
FindSolidity
--------

Find the Solidity internal includes and library.

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` targets, if
Solidity has been found.

``Solidity``
  Target for solidity static libraries and includes.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

::

  Solidity_INCLUDE_DIRS   - where to find Solidity internal headers
  Solidity_LIBRARIES      - List of internal libraries when using Solidity.
  Solidity_FOUND          - True if Solidity found.

Backward Compatibility
^^^^^^^^^^^^^^^^^^^^^^

The following variable are provided for backward compatibility

::

    TODO

Hints
^^^^^

Setting ``Solidity_ROOT`` or ``Solidity_LIB_DIR`` and ``Solidity_INCLUDE_DIRS`` respectively to solidity installation dirs to tell this
module where to look is suggested. Otherwise, this modules looks for solidity products in default paths.
#]=======================================================================]


set(_SOLIDITY_LIB_SEARCHES)
set(_SOLIDITY_INCLUDE_SEARCHES)

# Check if environment variables are set, also look for SOLIDITY_ROOT and SOLIDITYROOT as alternatives.
if ("$ENV{Solidity_ROOT}" STREQUAL "")
    if (NOT "$ENV{SOLIDITY_ROOT}" STREQUAL "")
        list(APPEND _SOLIDITY_LIB_SEARCHES $ENV{SOLIDITY_ROOT} NO_DEFAULT_PATH)
        list(APPEND _SOLIDITY_INCLUDE_SEARCHES $ENV{SOLIDITY_ROOT} NO_DEFAULT_PATH)
    elseif (NOT "$ENV{SOLIDITYROOT}" STREQUAL "")
        list(APPEND _SOLIDITY_LIB_SEARCHES $ENV{SOLIDITYROOT} NO_DEFAULT_PATH)
        list(APPEND _SOLIDITY_INCLUDE_SEARCHES $ENV{SOLIDITYROOT} NO_DEFAULT_PATH)
    else() # Check whether respective lib and include dirs are set
        if (NOT "$ENV{Solidity_LIB_DIR}" STREQUAL "")
            list(APPEND _SOLIDITY_LIB_SEARCHES $ENV{Solidity_LIB_DIR} NO_DEFAULT_PATH)
        endif()

        if (NOT "$ENV{Solidity_INCLUDE_DIRS}" STREQUAL "")
            list(APPEND _SOLIDITY_INCLUDE_SEARCHES $ENV{Solidity_INCLUDE_DIRS} NO_DEFAULT_PATH)
        endif()
    endif()
else()
    list(APPEND _SOLIDITY_LIB_SEARCHES $ENV{Solidity_ROOT} NO_DEFAULT_PATH)
    list(APPEND _SOLIDITY_INCLUDE_SEARCHES $ENV{Solidity_ROOT} NO_DEFAULT_PATH)
endif()

# Find Solidity header files
if (_SOLIDITY_INCLUDE_SEARCHES)
    foreach(search ${_SOLIDITY_INCLUDE_SEARCHES})
        find_path(Solidity_INCLUDE_DIRS NAMES libsolidity ${search} PATH_SUFFIXES include)
    endforeach()
else()
    find_path(Solidity_INCLUDE_DIRS NAMES libsolidity PATH_SUFFIXES include)
endif()

# Find Solidity static libraries
foreach(LIB_NAME evmasm langutil smtutil solc solidity solutil yul jsoncpp)
    if(_SOLIDITY_LIB_SEARCHES)
        foreach(search ${_SOLIDITY_LIB_SEARCHES})
            find_library(SOLIDITY_FOUND_LIB_${LIB_NAME}
                NAMES ${LIB_NAME}
                PATHS ${search}
                PATH_SUFFIXES lib
            )
            if(SOLIDITY_FOUND_LIB_${LIB_NAME})
                list(APPEND Solidity_LIBRARIES ${SOLIDITY_FOUND_LIB_${LIB_NAME}})
            endif()
        endforeach()
    else()
        find_library(SOLIDITY_FOUND_LIB_${LIB_NAME}
            NAMES ${LIB_NAME}
            PATH_SUFFIXES lib
        )
        if(SOLIDITY_FOUND_LIB_${LIB_NAME})
            list(APPEND Solidity_LIBRARIES ${SOLIDITY_FOUND_LIB_${LIB_NAME}})
        endif()
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set the SOLIDITY_FOUND variable
find_package_handle_standard_args(Solidity DEFAULT_MSG
    Solidity_LIBRARIES
    Solidity_INCLUDE_DIRS
)

if(Solidity_FOUND)
    message(STATUS "Found Solidity_LIBRARIES: ${Solidity_LIBRARIES}")
    message(STATUS "Found Solidity_INCLUDE_DIRS: ${Solidity_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Could not find Solidity libraries and headers")
endif()

add_library(Solidity INTERFACE IMPORTED)
if(Solidity_INCLUDE_DIRS)
    set_target_properties(Solidity PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Solidity_INCLUDE_DIRS}")
endif()

if(Solidity_LIBRARIES)
    set_target_properties(Solidity PROPERTIES
    INTERFACE_LINK_LIBRARIES "${Solidity_LIBRARIES}")
endif()
