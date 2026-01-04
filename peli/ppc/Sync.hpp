// peli/ppc/Sync.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../rt/SystemCall.hpp"

namespace peli::ppc {

inline void Sync() noexcept { __asm__ __volatile__("sync;"); }

inline void ISync() noexcept { __asm__ __volatile__("isync;"); }

inline void Eieio() noexcept { __asm__ __volatile__("eieio;"); }

inline void SyncBroadcast() {
  // Call sync syscall
  rt::SystemCall::Sync();
}

} // namespace peli::ppc