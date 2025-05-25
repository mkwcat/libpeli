// wsh/util/Debug.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdio>
#include <cstdlib>
#include <source_location>

namespace wsh::util {

#if defined(NDEBUG)

constexpr bool Debug = false;

#else // if !defined(NDEBUG)

constexpr bool Debug = true;

#endif

constexpr bool
Assert(bool condition, const char *message = nullptr,
       std::source_location location = std::source_location::current()) {
  if constexpr (Debug) {
    if (condition) {
      return true;
    }

    std::printf("Assertion failed%s%s\n"
                "In: %s\n"
                "File: %s\n"
                "Line: %d\n",
                message ? ": " : "!", message ? message : "",
                location.function_name(), location.file_name(),
                location.line());
    std::abort();
    return false;
  } else {
    if (!condition) {
      __builtin_trap();
    }
    return condition;
  }
}

} // namespace wsh::util