cmake_minimum_required(VERSION 3.15)
project(Solidity2CPN)
set(TARGET s2c_modules)
set(S2C_MODULE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_MODULE_BASE_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/base/include)
set(S2C_MODULE_PARSE_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/parse/include)
set(S2C_MODULE_NETGEN_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/netgen/include)

set(S2C_MODULE_BASE_SRC_DIR ${S2C_MODULE_ROOT_DIR}/base/src)
set(S2C_MODULE_PARSE_SRC_DIR ${S2C_MODULE_ROOT_DIR}/parse/src)
set(S2C_MODULE_NETGEN_SRC_DIR ${S2C_MODULE_ROOT_DIR}/netgen/src)

# env 
set(CMAKE_CXX_STANDARD 20)

# src & include
set(S2C_MODULE_HDR
    # base
    ${S2C_MODULE_BASE_INCLUDE_DIR}/base.hpp
    ${S2C_MODULE_BASE_INCLUDE_DIR}/cli.hpp
    ${S2C_MODULE_BASE_INCLUDE_DIR}/logger.hpp
    ${S2C_MODULE_BASE_INCLUDE_DIR}/singleton.hpp
    # parse
    ${S2C_MODULE_PARSE_INCLUDE_DIR}/parse.hpp
    # netgen
    ${S2C_MODULE_NETGEN_INCLUDE_DIR}/generator.hpp
)

set(S2C_MODULE_SRC
    # base
    ${S2C_MODULE_BASE_SRC_DIR}/logger.cpp
    ${S2C_MODULE_BASE_SRC_DIR}/cli.cpp
    # parse
    ${S2C_MODULE_PARSE_SRC_DIR}/parse.cpp
    # netgen
)

# deps
if (WIN32)
	# Boost 1.77 fixes a bug that causes crashes on Windows for some relative paths in --allow-paths.
	# See https://github.com/boostorg/filesystem/issues/201
	find_package(Boost 1.77.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})
else()
	# Boost 1.65 is the first to also provide boost::get for rvalue-references (#5787).
	find_package(Boost 1.65.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})
endif()

if (NOT TARGET Boost::boost) # header only target
	add_library(Boost::boost INTERFACE IMPORTED)
	set_property(TARGET Boost::boost APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()

# target
add_library(${TARGET} 
    ${S2C_MODULE_HDR} 
    ${S2C_MODULE_SRC} 
)

target_include_directories(${TARGET} PUBLIC 
    ${S2C_MODULE_BASE_INCLUDE_DIR}
    ${S2C_MODULE_PARSE_INCLUDE_DIR}
    ${S2C_MODULE_NETGEN_INCLUDE_DIR}
    # nlohmann::json
    ${S2C_THIRD_PARTY_DIR}/json/single_include/
)

target_link_libraries(${TARGET} PUBLIC
    solidity
    Boost::boost 
)
