cmake_minimum_required(VERSION 3.15)

project(Solidity2CPN)

set(S2C_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(S2C_SRC
    S2C_SRC_DIR/
)

add_executable(Solidity2CPN ${S2C_SRC} ${S2C_ROOT}/main.cpp)