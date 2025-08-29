cmake_minimum_required(VERSION 3.23)

project(wsh LANGUAGES C CXX ASM)

include_directories(wsh/..)

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

add_library(wsh STATIC
        wsh/hw/Video.cpp
        wsh/hw/VideoMode.cpp

        wsh/ios/low/Ipc.cpp

        wsh/ppc/Context.cpp
        wsh/ppc/Msr.cpp

        wsh/runtime/Args.cpp
        wsh/runtime/Crt0.cpp
        wsh/runtime/Exception.cpp
        wsh/runtime/GThread.cpp
        wsh/runtime/Memory.cpp
        wsh/runtime/Mutex.cpp
        wsh/runtime/Once.cpp
        wsh/runtime/Thread.cpp

        wsh/util/VIConsole.cpp
        wsh/util/VIConsoleFont.c
        wsh/util/VIConsoleStdOut.cpp
)