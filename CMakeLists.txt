cmake_minimum_required(VERSION 3.23)

project(wsh LANGUAGES C CXX ASM)

include_directories(wsh/..)

# Binary linker stuff
set(BIN2S ${DEVKITPRO}/tools/bin/bin2s)
set(BINARY_AS ${DEVKITPPC}/bin/powerpc-eabi-as)

set(MACHDEP "-DWSH_HOST_PPC -Wa,-mbroadway -mrvl -mcpu=750 -meabi -mhard-float")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_ASM_FLAGS "${MACHDEP} -x assembler-with-cpp")
set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-function -O4 -fstrict-volatile-bitfields ${CMAKE_C_FLAGS} ${MACHDEP}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-function -O4 -fstrict-volatile-bitfields -finline-limit=1400 -fno-threadsafe-statics ${CMAKE_CXX_FLAGS} ${MACHDEP}")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

add_library(wsh STATIC
        # wsh/ogc/aram.c
        # wsh/ogc/argv.c
        # wsh/ogc/arqmgr.c
        # wsh/ogc/arqueue.c
        # wsh/ogc/audio.c
        # wsh/ogc/cache_asm.S
        # wsh/ogc/cache.c
        # wsh/ogc/card.c
        # wsh/ogc/conf.c
        # wsh/ogc/decrementer_handler.S
        # wsh/ogc/decrementer.c
        # wsh/ogc/depackrnc.S
        # wsh/ogc/depackrnc1.c
        # wsh/ogc/dsp.c
        # wsh/ogc/dvd.c
        # wsh/ogc/es.c
        # wsh/ogc/exception_handler.S
        # wsh/ogc/exception.c
        # wsh/ogc/exi.c
        # wsh/ogc/gu_psasm.S
        # wsh/ogc/gu.c
        # wsh/ogc/gx.c
        # wsh/ogc/ios.c
        # wsh/ogc/ipc.c
        # wsh/ogc/irq_handler.S
        # wsh/ogc/irq.c
        # wsh/ogc/isfs.c
        # wsh/ogc/kprintf.c
        # wsh/ogc/lock_supp.c
        # wsh/ogc/malloc_lock.c
        # wsh/ogc/newlibc.c
        # wsh/ogc/pad.c
        # wsh/ogc/sdgecko_buf.c
        # wsh/ogc/sdgecko_io.c
        # wsh/ogc/si.c
        # wsh/ogc/stm.c
        # wsh/ogc/sys_state.c
        # wsh/ogc/system_asm.S
        # wsh/ogc/system.cpp
        # wsh/ogc/texconv.c
        # wsh/ogc/timesupp.c
        # wsh/ogc/tpl.c
        # wsh/ogc/usbgecko.c
        # wsh/ogc/video_asm.S
        # wsh/ogc/video.c
        # wsh/ogc/wiilaunch.c

        wsh/ios/ipc/Low.cpp

        wsh/ppc/Cache.cpp
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

        wsh/util/ViConsole.cpp
        wsh/util/ViConsoleFont.c
        wsh/util/ViConsoleStdOut.cpp
)