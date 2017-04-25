cmake_minimum_required(VERSION 3.5)

project(Compiler)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -DDEBUG")

add_subdirectory(seu_yacc)
add_subdirectory(seu_lex)
add_subdirectory(test)
