cmake_minimum_required(VERSION 3.16)
project(calc C)

set(CMAKE_C_STANDARD 11)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fexec-charset=GBK")
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

add_library(token INTERFACE)
target_sources(token INTERFACE token.h)

add_library(error_handle INTERFACE)
target_sources(error_handle INTERFACE error_handle.h)

add_library(lexer lexer.c)
add_library(parser parser.c)
add_library(vm vm.c)

link_libraries(parser lexer token error_handle vm)

add_executable(calc main.c)
