// wsh/runtime/Args.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::runtime {

struct Args {
  void Build(Args *input_args = nullptr) noexcept;

  int magic = 0;
  char *cmdLine = nullptr;
  int cmdLineSize = 0;

  // Output from build_argv
  int argc = 0;
  char **argv = nullptr;
  char **argvEnd = nullptr;
};

void libsysbase_build_argv(Args *argstruct) asm("build_argv");

} // namespace wsh::runtime