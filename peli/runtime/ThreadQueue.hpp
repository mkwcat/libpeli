// peli/runtime/ThreadQueue.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Thread.hpp"

namespace peli::runtime {

class ThreadQueue : protected Thread::List {
  friend class Thread;

public:
  constexpr ThreadQueue() : List({nullptr, nullptr}) {}

  void Enqueue(Thread *thread = Thread::GetCurrent()) {
    if (thread) {
      EnqueueTail<&Thread::m_wait_link>(thread);
    }
  }
  void Sleep() { Thread::Sleep(this); }
  void WakeupAll() { Thread::WakeupAll(this); }
  void WakeupOne() {
    Thread *thread = head;
    if (thread) {
      thread->Wakeup();
    }
  }
};

} // namespace peli::runtime