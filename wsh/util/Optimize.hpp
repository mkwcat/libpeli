// wsh/util/Optimize.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Debug.hpp"

namespace wsh::util {

constexpr bool
Assume(bool condition = true,
       std::source_location location = std::source_location::current()) {
  if constexpr (Debug) {
    return Assert(condition, nullptr, location);
  } else {
    [[assume(condition)]];
    return condition;
  }
}

[[__noreturn__]]
constexpr void Unreachable() noexcept {
  __builtin_unreachable();
}

} // namespace wsh::util