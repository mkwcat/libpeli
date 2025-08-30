// peli/runtime/Cond.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../ppc/Msr.hpp"
#include "Mutex.hpp"

namespace peli::runtime {

class Cond {
public:
  Cond() noexcept : m_wait_queue({}) {}

  ~Cond() = default;

  void Wait(Mutex *mutex) noexcept {
    ppc::Msr::NoInterruptsScope guard;

    m_wait_queue.Enqueue();
    mutex->Unlock();
    // Unlock may incur a context switch, regardless of the interrupt state, so
    // check to ensure we're still on the queue
    if (auto current = Thread::GetCurrent()) {
      if (current->GetWaitQueue() == &m_wait_queue) {
        // Still on the queue, so we can sleep
        current->Sleep();
      }
    }
    mutex->Lock();
  }

  void Signal() noexcept {
    ppc::Msr::NoInterruptsScope guard;

    m_wait_queue.WakeupOne();
  }

  void Broadcast() noexcept {
    ppc::Msr::NoInterruptsScope guard;

    m_wait_queue.WakeupAll();
  }

private:
  ThreadQueue m_wait_queue;
};

} // namespace peli::runtime