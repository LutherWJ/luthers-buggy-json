cmake_minimum_required(VERSION 3.20)
project(RTOS_DEMO2 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(json_parser)

target_sources(json_parser PRIVATE
    src/main.c
    src/json_parser.c
    src/json_lexer.c 
    src/array_list.c
)

target_include_directories(json_parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

