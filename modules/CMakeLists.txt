cmake_minimum_required(VERSION 3.15)
project(Plasm)
set(TARGET psm_modules)
set(PSM_MODULE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PSM_MODULE_BASE_INCLUDE_DIR ${PSM_MODULE_ROOT_DIR}/base/include)
set(PSM_MODULE_CORE_INCLUDE_DIR ${PSM_MODULE_ROOT_DIR}/core/include)

set(PSM_MODULE_BASE_SRC_DIR ${PSM_MODULE_ROOT_DIR}/base/src)
set(PSM_MODULE_CORE_SRC_DIR ${PSM_MODULE_ROOT_DIR}/core/src)

# src & include
set(PSM_MODULE_HDR
    # base
    ${PSM_MODULE_BASE_INCLUDE_DIR}/base.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/cli.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/logger.hpp
    ${PSM_MODULE_BASE_INCLUDE_DIR}/singleton.hpp
    # core
    ${PSM_MODULE_CORE_INCLUDE_DIR}/noopgenerator.hpp
    ${PSM_MODULE_CORE_INCLUDE_DIR}/cpnidegenerator.hpp
    ${PSM_MODULE_CORE_INCLUDE_DIR}/cpnxml.hpp
    ${PSM_MODULE_CORE_INCLUDE_DIR}/generator.hpp
    ${PSM_MODULE_CORE_INCLUDE_DIR}/cpn.hpp
)

set(PSM_MODULE_SRC
    # base
    ${PSM_MODULE_BASE_SRC_DIR}/logger.cpp
    ${PSM_MODULE_BASE_SRC_DIR}/cli.cpp
    # core
    ${PSM_MODULE_CORE_SRC_DIR}/noopgenerator.cpp
    ${PSM_MODULE_CORE_SRC_DIR}/cpnidegenerator.cpp
    ${PSM_MODULE_CORE_SRC_DIR}/cpnxml.cpp
    ${PSM_MODULE_CORE_SRC_DIR}/generator.cpp
    ${PSM_MODULE_CORE_SRC_DIR}/cpn.cpp
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
    # OGDF
    Boost::boost
    Boost::filesystem
    Boost::program_options
)
