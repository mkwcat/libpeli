// peli/runtime/SystemCall.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::runtime::SystemCall {

namespace detail {
extern void SystemCallHandler() noexcept;
}

inline void Sync() noexcept {
  __asm__ __volatile__("crclr 28; crclr 29; sc" : : : "cr7", "r0", "r3");
}
inline void DisableInterrupts() noexcept {
  __asm__ __volatile__("crset 28; sc" : : : "cr7", "r0");
}
inline void DcLockAndFlush() noexcept {
  __asm__ __volatile__("crclr 28; crset 29; sc" : : : "cr7", "r0", "r3");
}

} // namespace peli::runtime::SystemCall