// wsh/util/Halt.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::util {

[[__noreturn__]]
static inline void Halt() noexcept {
  asm volatile("b .");
  __builtin_unreachable();
}

} // namespace wsh::util