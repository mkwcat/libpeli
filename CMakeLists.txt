cmake_minimum_required(VERSION 3.23)

project(peli LANGUAGES C CXX ASM)

include_directories(peli/..)

# Binary linker stuff
set(BIN2S ${DEVKITPRO}/tools/bin/bin2s)
set(BINARY_AS ${DEVKITPPC}/bin/powerpc-eabi-as)

set(MACHDEP "-Wa,-mbroadway -mrvl -mcpu=750 -meabi -mhard-float")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_ASM_FLAGS "${MACHDEP} -x assembler-with-cpp")
set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-function -O4 -fstrict-volatile-bitfields ${CMAKE_C_FLAGS} ${MACHDEP}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-function -O4 -fstrict-volatile-bitfields -finline-limit=1400 -fno-threadsafe-statics ${CMAKE_CXX_FLAGS} ${MACHDEP}")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

add_library(peli STATIC
        peli/hw/Video.cpp
        peli/hw/VideoMode.cpp

        peli/ios/low/Ipc.cpp

        peli/ppc/Context.cpp
        peli/ppc/Msr.cpp

        peli/runtime/Args.cpp
        peli/runtime/Crt0.cpp
        peli/runtime/Exception.cpp
        peli/runtime/GThread.cpp
        peli/runtime/Memory.cpp
        peli/runtime/Mutex.cpp
        peli/runtime/Once.cpp
        peli/runtime/Thread.cpp

        peli/util/VIConsole.cpp
        peli/util/VIConsoleFont.c
        peli/util/VIConsoleStdOut.cpp
)