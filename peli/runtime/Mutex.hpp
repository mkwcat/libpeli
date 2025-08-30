// peli/runtime/Mutex.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "Thread.hpp"
#include "ThreadQueue.hpp"

namespace peli::runtime {

class Mutex {
public:
  constexpr Mutex() noexcept
      : m_owner_thread(nullptr), m_wait_queue({}), m_lock_count(0),
        m_recursive(false) {}
  ~Mutex() noexcept;

  Mutex(const Mutex &) = delete;
  Mutex(Mutex &&) = delete;

  void Lock() noexcept;

  void Unlock() noexcept;

  bool TryLock() noexcept;

protected:
  Thread *m_owner_thread;
  ThreadQueue m_wait_queue;
  u32 m_lock_count;
  bool m_recursive;
};

class RecursiveMutex : public Mutex {
public:
  constexpr RecursiveMutex() : Mutex() { m_recursive = true; }
};

} // namespace peli::runtime