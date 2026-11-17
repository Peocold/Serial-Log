add_executable(serial_log main.c)

add_library(serial_lib load.c
                       print.c)

target_include_directories(serial_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(serial_log PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_link_libraries(serial_log PRIVATE serial_lib)