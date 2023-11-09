cmake_minimum_required(VERSION 3.15)
project(Plasm)

function(add_test NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_link_libraries(${NAME} PUBLIC psm_modules)
endfunction()

add_test(cpnxml_test)
