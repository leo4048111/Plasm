cmake_minimum_required(VERSION 3.15)
project(Solidity2CPN)
set(TARGET s2c_modules)
set(S2C_MODULE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_MODULE_BASE_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/base/include)
set(S2C_MODULE_NETGEN_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/netgen/include)

set(S2C_MODULE_BASE_SRC_DIR ${S2C_MODULE_ROOT_DIR}/base/src)
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
    # netgen
    ${S2C_MODULE_NETGEN_INCLUDE_DIR}/generator.hpp
)

set(S2C_MODULE_SRC
    # base
    ${S2C_MODULE_BASE_SRC_DIR}/logger.cpp
    ${S2C_MODULE_BASE_SRC_DIR}/cli.cpp
    # netgen
)

# target
add_library(${TARGET} 
    ${S2C_MODULE_HDR} 
    ${S2C_MODULE_SRC} 
)

target_include_directories(${TARGET} PUBLIC 
    ${S2C_MODULE_BASE_INCLUDE_DIR}
    ${S2C_MODULE_NETGEN_INCLUDE_DIR}
)

target_link_libraries(${TARGET} PUBLIC
    solidity
    Boost::boost
    Boost::filesystem
    Boost::program_options
)
