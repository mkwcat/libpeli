// wsh/util/Halt.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

#if defined(WSH_DEBUG)
#include <stdio.h>
#endif

namespace wsh::util {

[[__noreturn__]]
static inline void Halt() noexcept {
  asm volatile("b .");
  __builtin_unreachable();
}

#if defined(WSH_DEBUG)

#define _WSH_ASSERT_2(X_CONDITION, X_LINE, ...)                                \
  while (!(X_CONDITION)) {                                                     \
    ::wsh::util::AssertFail("(" #X_CONDITION ") " __VA_OPT__(, ) __VA_ARGS__,  \
                            __PRETTY_FUNCTION__, __FILE__, #X_LINE);           \
  }
#define _WSH_ASSERT_1(X_CONDITION, X_LINE, ...)                                \
  _WSH_ASSERT_2(X_CONDITION, X_LINE, __VA_ARGS__)
#define _WSH_ASSERT(X_CONDITION, ...)                                          \
  _WSH_ASSERT_1(X_CONDITION, __LINE__, __VA_ARGS__)

#define _WSH_DEBUG_ASSERT(X_CONDITION, ...)                                    \
  _WSH_ASSERT(X_CONDITION, __VA_ARGS__)

#define _WSH_PANIC_2(X_MESSAGE, X_LINE)                                        \
  ::wsh::util::Panic(X_MESSAGE, __PRETTY_FUNCTION__, __FILE__, #X_LINE)
#define _WSH_PANIC_1(X_MESSAGE, X_LINE) _WSH_PANIC_2(X_MESSAGE, X_LINE)
#define _WSH_PANIC(X_MESSAGE) _WSH_PANIC_1(X_MESSAGE, __LINE__)

[[__noreturn__]]
inline void AssertFail(const char *condition, const char *message,
                       const char *function, const char *file,
                       const char *line) noexcept {
  puts("Assertion ");
  if (condition) {
    puts(condition);
  }
  puts(" failed");
  if (message) {
    puts(": ");
    puts(message);
  }
  puts("\nIn: ");
  puts(function);
  puts("\nFile: ");
  puts(file);
  puts("\nLine: ");
  puts(line);
  puts("\n");

  __builtin_trap();
  __builtin_unreachable();
}

[[__noreturn__]]
inline void AssertFail(const char *condition, const char *function,
                       const char *file, const char *line) noexcept {
  AssertFail(condition, nullptr, function, file, line);
}

[[__noreturn__]]
inline void Panic(const char *message, const char *function, const char *file,
                  const char *line) noexcept {
  puts("Panic: ");
  puts(message);
  puts("\nIn: ");
  puts(function);
  puts("\nFile: ");
  puts(file);
  puts("\nLine: ");
  puts(line);
  puts("\n");

  __builtin_trap();
  __builtin_unreachable();
}

#else // WSH_DEBUG

#define _WSH_ASSERT(X_CONDITION, ...)                                          \
  while (!(X_CONDITION)) {                                                     \
    __builtin_trap();                                                          \
    __builtin_unreachable();                                                   \
  }

#define _WSH_DEBUG_ASSERT(X_CONDITION, ...)                                    \
  do {                                                                         \
  } while (false)

#define _WSH_PANIC(X_MESSAGE)                                                  \
  do {                                                                         \
    __builtin_trap();                                                          \
    __builtin_unreachable();                                                   \
  } while (false)

#endif

} // namespace wsh::util