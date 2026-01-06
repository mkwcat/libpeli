set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR powerpc-eabi)

file(TO_CMAKE_PATH "$ENV{DEVKITPRO}" DEVKITPRO)
file(TO_CMAKE_PATH "$ENV{DEVKITPPC}" DEVKITPPC)

if(NOT DEVKITPPC)
    message(FATAL_ERROR "Please set DEVKITPPC in your environment")
endif()

if(NOT DEVKITPRO)
    message(FATAL_ERROR "Please set DEVKITPRO in your environment")
endif()

if(WIN32)
    set(CMAKE_C_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-gcc.exe)
    set(CMAKE_CXX_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-g++.exe)
    set(CMAKE_AR ${DEVKITPPC}/bin/powerpc-eabi-ar.exe)
    set(CMAKE_RANLIB ${DEVKITPPC}/bin/powerpc-eabi-ranlib.exe)
else()
    set(CMAKE_C_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-gcc)
    set(CMAKE_CXX_COMPILER ${DEVKITPPC}/bin/powerpc-eabi-g++)
    set(CMAKE_AR ${DEVKITPPC}/bin/powerpc-eabi-ar)
    set(CMAKE_RANLIB ${DEVKITPPC}/bin/powerpc-eabi-ranlib)
endif()

set(CMAKE_FIND_ROOT_PATH ${DEVKITPPC})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(MACHDEP -D__POWERPC__ -Wa,-mbroadway -mrvl -mcpu=750 -meabi -mhard-float)