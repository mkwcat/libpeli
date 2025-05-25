// wsh/runtime/Once.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"

namespace wsh::runtime {

class OnceControl {
public:
  class Guard {
  public:
    Guard(OnceControl &control) noexcept : m_control(control) {}

    operator bool() const noexcept { return m_control.Once(); }

    ~Guard() noexcept { m_control.Done(); }

  private:
    OnceControl &m_control;
  };

  constexpr OnceControl() noexcept : m_state(State::Uninitialized) {}

  bool Once() noexcept;
  void Done() noexcept;

private:
  enum class State : u8 {
    Uninitialized = 0,
    Initialized,
    InProgress,
  };

  State m_state;
};

} // namespace wsh::runtime