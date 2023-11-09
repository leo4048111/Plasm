cmake_minimum_required(VERSION 3.15)
project(Plasm)
set(TARGET psm_modules)
set(PSM_MODULE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PSM_MODULE_BASE_INCLUDE_DIR ${PSM_MODULE_ROOT_DIR}/base/include)
set(PSM_MODULE_CORE_INCLUDE_DIR ${PSM_MODULE_ROOT_DIR}/core/include)

set(PSM_MODULE_BASE_SRC_DIR ${PSM_MODULE_ROOT_DIR}/base/src)
set(PSM_MODULE_CORE_SRC_DIR ${PSM_MODULE_ROOT_DIR}/core/src)

# env
set(CMAKE_CXX_STANDARD 20)

# src & include
set(PSM_MODULE_HDR
    # base
    ${PSM_MODULE_BASE_INCLUDE_DIR}/base.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/cli.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/logger.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/singleton.hpp
    # core
    ${PSM_MODULE_CORE_INCLUDE_DIR}/generator.hpp
)

set(PSM_MODULE_SRC
    # base
    ${PSM_MODULE_BASE_SRC_DIR}/logger.cpp
    ${PSM_MODULE_BASE_SRC_DIR}/cli.cpp
    # core
    ${PSM_MODULE_CORE_SRC_DIR}/generator.cpp
)

# target
add_library(${TARGET}
    ${PSM_MODULE_HDR}
    ${PSM_MODULE_SRC}
)

target_include_directories(${TARGET} PUBLIC
    ${PSM_MODULE_BASE_INCLUDE_DIR}
    ${PSM_MODULE_CORE_INCLUDE_DIR}
)

target_link_libraries(${TARGET} PUBLIC
    solidity
    pugixml
    Boost::boost
    Boost::filesystem
    Boost::program_options
)
