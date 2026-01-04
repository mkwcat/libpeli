// peli/rt/Mutex.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Mutex.hpp"
#include "../ppc/Msr.hpp"
#include "../util/Halt.hpp"

namespace peli::rt {

Mutex::~Mutex() noexcept {
  _PELI_ASSERT(m_lock_count == 0, "Mutex destroyed while locked");
}

void Mutex::Lock() noexcept {
  Thread *current = Thread::GetCurrent();

  ppc::Msr::NoInterruptsScope guard;
  if (m_lock_count > 0) {
    if (m_recursive && m_owner_thread == current) {
      m_lock_count++;
      return;
    }
    // Wait for the mutex to be unlocked
    m_wait_queue.Sleep();
  }

  // Lock the mutex
  m_lock_count++;
  m_owner_thread = current;
}

void Mutex::Unlock() noexcept {
  ppc::Msr::NoInterruptsScope guard;

  _PELI_ASSERT(m_lock_count > 0, "Attempt to unlock a mutex that is not locked");

  if (--m_lock_count > 0) {
    return;
  }

  // Unlock the mutex
  // Wake up the next thread waiting for the mutex
  m_wait_queue.WakeupOne();
}

bool Mutex::TryLock() noexcept {
  Thread *current = Thread::GetCurrent();

  ppc::Msr::NoInterruptsScope guard;
  if (m_lock_count > 0) {
    if (m_recursive && m_owner_thread == current) {
      m_lock_count++;
      return true;
    }
    return false;
  }

  // Lock the mutex
  m_lock_count++;
  m_owner_thread = current;
  return true;
}

} // namespace peli::rt