cmake_minimum_required(VERSION 3.23)

include(toolchain-devkitppc.cmake)

project(peli LANGUAGES C CXX ASM)

include_directories(peli/..)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

file(GLOB_RECURSE SOURCE_FILES peli/*.c peli/*.cpp)
add_library(peli STATIC ${SOURCE_FILES})

target_compile_options(peli PRIVATE
    ${MACHDEP}

    -g
    -msdata=eabi
    -Wall
    -Wextra
    -Wpedantic
    -ffunction-sections
    -fdata-sections
    -O3
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-finline-limit=1400>
)
    
target_compile_options(peli PUBLIC
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fstrict-volatile-bitfields>
)
