// peli/util/Halt.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

#if defined(PELI_DEBUG)
#include <stdio.h>
#endif

namespace peli::util {

[[__noreturn__]]
static inline void Halt() noexcept {
  asm volatile("b .");
  __builtin_unreachable();
}

#if defined(PELI_DEBUG)

#define _PELI_ASSERT_2(X_CONDITION, X_LINE, ...)                                \
  while (!(X_CONDITION)) {                                                     \
    ::peli::util::AssertFail("(" #X_CONDITION ") " __VA_OPT__(, ) __VA_ARGS__,  \
                            __PRETTY_FUNCTION__, __FILE__, #X_LINE);           \
  }
#define _PELI_ASSERT_1(X_CONDITION, X_LINE, ...)                                \
  _PELI_ASSERT_2(X_CONDITION, X_LINE, __VA_ARGS__)
#define _PELI_ASSERT(X_CONDITION, ...)                                          \
  _PELI_ASSERT_1(X_CONDITION, __LINE__, __VA_ARGS__)

#define _PELI_DEBUG_ASSERT(X_CONDITION, ...)                                    \
  _PELI_ASSERT(X_CONDITION, __VA_ARGS__)

#define _PELI_PANIC_2(X_MESSAGE, X_LINE)                                        \
  ::peli::util::Panic(X_MESSAGE, __PRETTY_FUNCTION__, __FILE__, #X_LINE)
#define _PELI_PANIC_1(X_MESSAGE, X_LINE) _PELI_PANIC_2(X_MESSAGE, X_LINE)
#define _PELI_PANIC(X_MESSAGE) _PELI_PANIC_1(X_MESSAGE, __LINE__)

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

#else // PELI_DEBUG

#define _PELI_ASSERT(X_CONDITION, ...)                                          \
  while (!(X_CONDITION)) {                                                     \
    __builtin_trap();                                                          \
    __builtin_unreachable();                                                   \
  }

#define _PELI_DEBUG_ASSERT(X_CONDITION, ...)                                    \
  do {                                                                         \
  } while (false)

#define _PELI_PANIC(X_MESSAGE)                                                  \
  do {                                                                         \
    __builtin_trap();                                                          \
    __builtin_unreachable();                                                   \
  } while (false)

#endif

} // namespace peli::util