cmake_minimum_required(VERSION 3.13)

project(SimpleApp)

link_directories(../../build)
include_directories(../..)

# Binary linker stuff
set(BIN2S ${DEVKITPRO}/tools/bin/bin2s)
set(BINARY_AS ${DEVKITPPC}/bin/powerpc-eabi-as)

set(MACHDEP "-mrvl -mcpu=750 -meabi -mhard-float")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")
set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-function -O2 ${CMAKE_CXX_FLAGS} ${MACHDEP}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-function -O2 ${CMAKE_CXX_FLAGS} ${MACHDEP} -fdiagnostics-color")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_executable(SimpleApp
	SimpleApp.cpp
)

# link library with whole archive
target_link_libraries(SimpleApp PRIVATE
    "-Wl,--whole-archive"
    wsh
    "-Wl,--no-whole-archive"
)

target_link_options(SimpleApp PRIVATE
    "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/SimpleApp.map"
    "-Wl,--gc-sections"
)