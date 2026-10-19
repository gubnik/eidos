cmake_minimum_required(VERSION 3.31)

option(EIDOS_KILL_LIBC "Disables Eidos dependency on libc" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(eidos VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(EIDOS_LIB_NAME "eidos")
set(EIDOS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EIDOS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(EIDOS_KILL_LIBC)
    file(GLOB_RECURSE EIDOS_SOURCE_SET "${EIDOS_SOURCE_DIR}/eidos/*.c")
else()
    file(GLOB_RECURSE EIDOS_SOURCE_SET
        "${EIDOS_SOURCE_DIR}/eidos/*.c"
        "${EIDOS_SOURCE_DIR}/eidos_libc/*.c")
endif()

add_library(${EIDOS_LIB_NAME} ${EIDOS_SOURCE_SET})

if(EIDOS_KILL_LIBC)
    target_compile_definitions(${EIDOS_LIB_NAME} PUBLIC EIDOS_KILL_LIBC=1)
    target_compile_options(${EIDOS_LIB_NAME} PRIVATE "-nostdlib")
endif()

target_include_directories(${EIDOS_LIB_NAME}
    PUBLIC $<BUILD_INTERFACE:${EIDOS_INCLUDE_DIR}>)
