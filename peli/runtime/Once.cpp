// peli/runtime/Once.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Once.hpp"
#include "../ppc/Msr.hpp"
#include "Thread.hpp"

namespace peli::runtime {

bool OnceControl::Once() noexcept {
  ppc::Msr::NoInterruptsScope int_guard;
  if (m_state == State::Uninitialized) {
    m_state = State::InProgress;
    return true;
  }

  // If we are in progress, we need to wait for the other thread to finish
  while (m_state == State::InProgress) {
    Thread::Yield();
  }
  return false;
}

void OnceControl::Done() noexcept {
  if (m_state == State::InProgress) {
    m_state = State::Initialized;
  }
}

} // namespace peli::runtime