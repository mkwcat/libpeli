# libpeli

A core runtime library for Wii homebrew applications, written in C++23. Currently a work in progress.

Current working features include:
- PowerPC initialization
- Multithreading
- Exception handling
- Low-level IPC with IOS
- Video configuration

## Building

You will need devkitPPC and CMake. An end goal may be to make the library compatible with other toolchains and compilers,
but for right now this is all that's supported.

Clone the repository and then in the repository directory, run:

```
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain-powerpc.cmake
cmake --build .
```

The output is a static library named `libpeli.a` in the build directory. See the example project in [tests/simple](tests/simple)
for a simple demonstration on how to use the library.

## License

The project is made available under the MIT License. See the [LICENSE](LICENSE) file for the full text of the license.
