cmake_minimum_required(VERSION 3.23)

include(../toolchain-devkitppc.cmake)

project(peli-tests LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

link_directories(../build)
include_directories(..)

# link library with whole archive
link_libraries(
    "-Wl,--whole-archive"
    peli
    "-Wl,--no-whole-archive"
)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_compile_options(
    ${MACHDEP}

    -g
    -Wall
    -Wextra
    -Wpedantic
    -ffunction-sections
    -fdata-sections
    -fno-exceptions
    -fno-unwind-tables
    -O3
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fstrict-volatile-bitfields>
)

add_link_options(
    -Wl,--gc-sections
    -T../peli-ppc.ld
)

add_executable(SimpleApp SimpleApp.cpp)
add_executable(HardReset HardReset.cpp)
add_executable(SysConfReader SysConfReader.cpp)
add_executable(SpecialPurposeRegisters SpecialPurposeRegisters.cpp)
add_executable(SDCard SDCard.cpp)
add_executable(VideoConsole VideoConsole.cpp)
add_executable(Arguments Arguments.cpp)