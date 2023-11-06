cmake_minimum_required(VERSION 3.15)
project(Solidity2CPN)
set(TARGET Solidity2CPN)

# env
set(S2C_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_THIRD_PARTY_DIR ${S2C_ROOT_DIR}/third_party)
set(S2C_SRC_DIR ${S2C_ROOT_DIR}/src)

# target
add_executable(${TARGET} ${S2C_ROOT_DIR}/main.cpp)

# deps
target_include_directories(
    ${TARGET} 
    PRIVATE
    ${S2C_THIRD_PARTY_DIR}/json/single_include/
    )
