// peli/rt/Arguments.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::rt {

struct Arguments {
  static constexpr int Magic = 0x5F617267; // "_arg"

  void Build(Arguments *input_args = nullptr) noexcept;

  int m_magic = 0;
  char *m_command_line = nullptr;
  int m_command_line_size = 0;

  // Output from build_argv
  int m_argc = 0;
  char **m_argv = nullptr;
  char **m_argv_end = nullptr;
};

} // namespace peli::rt