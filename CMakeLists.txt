cmake_minimum_required(VERSION 3.15)
project(Plasm)
set(TARGET Plasm)

# env
set(CMAKE_CXX_STANDARD 20)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
if ("${LD_VERSION}" MATCHES "GNU gold")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif ()
set(PSM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PSM_THIRD_PARTY_DIR ${PSM_ROOT_DIR}/third_party)

# deps
list(APPEND CMAKE_MODULE_PATH ${PSM_ROOT_DIR}/cmake)

# boost
set(BOOST_COMPONENTS "filesystem;program_options;system")

if (WIN32)
	# Boost 1.77 fixes a bug that causes crashes on Windows for some relative paths in --allow-paths.
	# See https://github.com/boostorg/filesystem/issues/201
	find_package(Boost 1.77.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})
else()
	# Boost 1.65 is the first to also provide boost::get for rvalue-references (#5787).
	find_package(Boost 1.65.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})
endif()

if(Boost_FOUND)
    message(STATUS "Boost found")
else ()
    message(FATAL_ERROR "Boost not found")
endif()

if (NOT TARGET Boost::boost) # header only target
	add_library(Boost::boost INTERFACE IMPORTED)
	set_property(TARGET Boost::boost APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()

# # solidity internal
# find_package(Solidity REQUIRED)
# if(Solidity_FOUND)
#     message(STATUS "Solidity found")
# else ()
#     message(FATAL_ERROR "Solidity not found")
# endif()

# solidity
add_subdirectory(${PSM_THIRD_PARTY_DIR}/solidity)

# pugixml
add_subdirectory(${PSM_THIRD_PARTY_DIR}/pugixml)

# modules
add_subdirectory(${PSM_ROOT_DIR}/modules)

# target
add_executable(${TARGET} ${PSM_ROOT_DIR}/main.cpp)

# deps
target_link_libraries(
    ${TARGET}
    PRIVATE
    psm_modules
)
