cmake_minimum_required(VERSION 3.15)
project(Solidity2CPN)
set(TARGET s2c_modules)
set(S2C_MODULE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_MODULE_BASE_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/base/include)
set(S2C_MODULE_PARSE_INCLUDE_DIR ${S2C_MODULE_ROOT_DIR}/parse/include)

set(S2C_MODULE_BASE_SRC_DIR ${S2C_MODULE_ROOT_DIR}/base/src)
set(S2C_MODULE_PARSE_SRC_DIR ${S2C_MODULE_ROOT_DIR}/parse/src)

# env 
set(CMAKE_CXX_STANDARD 20)

# src & include
set(S2C_MODULE_HDR
    # base
    ${S2C_MODULE_BASE_INCLUDE_DIR}/base.hpp
    ${S2C_MODULE_BASE_INCLUDE_DIR}/logger.hpp
    ${S2C_MODULE_BASE_INCLUDE_DIR}/singleton.hpp
    # parse
    ${S2C_MODULE_PARSE_INCLUDE_DIR}/parse.hpp
)

set(S2C_MODULE_SRC
    # base
    ${S2C_MODULE_BASE_SRC_DIR}/logger.cpp
    # parse
    ${S2C_MODULE_PARSE_SRC_DIR}/parse.cpp
)

message(STATUS ${S2C_MODULE_SRC})

# target
add_library(${TARGET} ${S2C_MODULE_HDR} ${S2C_MODULE_SRC})
target_include_directories(${TARGET} PUBLIC 
    ${S2C_MODULE_BASE_INCLUDE_DIR}
    ${S2C_MODULE_PARSE_INCLUDE_DIR}
)
