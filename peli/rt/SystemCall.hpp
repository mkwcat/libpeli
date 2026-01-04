// peli/rt/SystemCall.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"

namespace peli::rt::SystemCall {

namespace detail {
extern void SystemCallHandler() noexcept;
}

inline void Sync() noexcept {
  __asm__ __volatile__("crclr 28; crclr 29; sc" : : : "cr7", "r11", "r12");
}
inline void DisableInterrupts() noexcept {
  __asm__ __volatile__("crset 28; sc" : : : "cr7", "r11");
}
inline void DcFlush(u32 hid0_set, u32 hid0_and) noexcept {
  register u32 hid0_set_pass asm("r11") = hid0_set;
  register u32 hid0_and_pass asm("r12") = hid0_and;
  __asm__ __volatile__("crclr 28; crset 29; sc"
                       : "+r"(hid0_set_pass), "+r"(hid0_and_pass)
                       :
                       : "cr7", "r10");
}

} // namespace peli::rt::SystemCall