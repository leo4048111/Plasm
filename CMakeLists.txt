cmake_minimum_required(VERSION 3.15)
project(Solidity2CPN)
set(TARGET Solidity2CPN)

# env
add_definitions(-D_GNU_SOURCE)
set(CMAKE_CXX_STANDARD 20)
set(S2C_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(S2C_THIRD_PARTY_DIR ${S2C_ROOT_DIR}/third_party)

# modules
add_subdirectory(${S2C_ROOT_DIR}/modules)

# target
add_executable(${TARGET} ${S2C_ROOT_DIR}/main.cpp)

# deps
target_include_directories(
    ${TARGET} 
    PRIVATE
    ${S2C_THIRD_PARTY_DIR}/json/single_include/
    )

target_link_libraries(
    ${TARGET}
    PRIVATE
    s2c_modules
)
