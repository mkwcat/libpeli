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
    -Werror
    -Wall
    -Wextra
    -Wconversion
    -Wpedantic
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -ffunction-sections
    -fdata-sections
    -Os
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
)
    
target_compile_options(peli PUBLIC
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fstrict-volatile-bitfields>
)
