// peli/tests/Arguments.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <peli/log/VideoConsole.hpp>
#include <peli/log/VideoConsoleStdOut.hpp>

int main(int argc, char **argv) {
  peli::log::VideoConsole console(false);

  console.Print("\nlibpeli! Arguments list:\n");

  // Register the console as stdout
  peli::log::VideoConsoleStdOut::Register(console);

  for (int i = 0; i < argc; i++) {
    std::printf("argv[%d]: %s\n", i, argv[i]);
  }

  return 0;
}