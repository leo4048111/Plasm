# FindSolidity.cmake
# Attempt to find Solidity static libraries and headers in common locations

# Find Solidity header files
find_path(Solidity_INCLUDE_DIR
          NAMES solidity
          PATHS ${CMAKE_INCLUDE_PATH}
          DOC "Directory where the Solidity headers are located"
)

# Find Solidity static libraries
foreach(lib_name evmasm langutil smtutil solc solidity solutil yul jsoncpp)
    find_library(FOUND_LIB_${lib_name}
        NAMES ${lib_name}
        PATHS /usr/local/lib/
        NO_DEFAULT_PATH
    )
    if(FOUND_LIB_${lib_name})
        list(APPEND Solidity_LIBRARY ${FOUND_LIB_${lib_name}})
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set the SOLIDITY_FOUND variable
find_package_handle_standard_args(Solidity DEFAULT_MSG
    Solidity_LIBRARY
    Solidity_INCLUDE_DIR
)

if(Solidity_FOUND)
    set(Solidity_LIBRARIES ${Solidity_LIBRARY})
    set(Solidity_INCLUDE_DIRS ${Solidity_INCLUDE_DIR}/solidity)
    message(STATUS "Solidity_LIBRARIES: ${Solidity_LIBRARIES}")
    message(STATUS "Solidity_INCLUDE_DIRS: ${Solidity_INCLUDE_DIRS}")
else()
    message(STATUS "Could not find Solidity libraries and headers")
endif()

add_library(Solidity INTERFACE)
target_include_directories(Solidity INTERFACE ${Solidity_INCLUDE_DIRS})
target_link_libraries(Solidity INTERFACE ${Solidity_LIBRARIES})
