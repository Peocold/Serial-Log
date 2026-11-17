add_executable(serial_log main.c)

add_library(serial_lib load.c
                       print.c)

target_include_directories(serial_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(serial_log PRIVATE serial_lib)