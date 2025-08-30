// peli/ppc/Context.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../ios/LoMem.hpp"

namespace peli::ppc {

class Context : public ios::OSContext {
public:
  [[noreturn]]
  void Restore() noexcept;

  /**
   * Save the current CPU context to this structure. Returns true on original
   * call, false if the context was restored.
   */
  bool Save() noexcept;

  void FastSwitch() noexcept;
};

} // namespace peli::ppc