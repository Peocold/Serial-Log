cmake_minimum_required(VERSION 3.13)
project(Serial_Log)

add_executable(serial_log main.c)

add_library(serial_lib load.c)

target_link_libraries(serial_log PRIVATE serial_lib)