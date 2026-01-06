// peli/rt/Arguments.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Arguments.hpp"
#include "../util/Address.hpp"
#include "Arena.hpp"

namespace peli::rt {

extern "C" {
// libsysbase
void build_argv(Arguments *argstruct);
}

void Arguments::Build([[maybe_unused]] Arguments *input_args) noexcept {
  if (!input_args) {
    input_args = this;
  }

  if (input_args->m_magic != Magic) {
    *this = {};
    return;
  }

  // TODO: Don't rely on libsysbase's build_argv implementation

  m_command_line = util::AlignUp(4, reinterpret_cast<char *>(Arena::Mem1Start));
  m_command_line_size = input_args->m_command_line_size;
  __builtin_memmove(m_command_line, input_args->m_command_line,
                    input_args->m_command_line_size);

  build_argv(this);

  m_magic = Magic;
  Arena::Mem1Start = reinterpret_cast<u8 *>(m_argv_end);
}

} // namespace peli::rt