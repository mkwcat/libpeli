// wsh/runtime/Args.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::runtime {

struct Args {
  void Build() noexcept;

  int magic;
  char *cmdLine;
  int cmdLineSize;

  // Output from build_argv
  int argc;
  char **argv;
  char **argvEnd;
};

void libsysbase_build_argv(Args *argstruct) asm("build_argv");

} // namespace wsh::runtime