// wsh/util/Optimize.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#if defined(WSH_DEBUG)
#include "Halt.hpp"
#endif

namespace wsh::util {

#if defined(WSH_DEBUG)

#define _WSH_ASSUME_2(X_CONDITION, X_LINE)                                     \
  do {                                                                         \
    if (!(X_CONDITION)) {                                                      \
      util::AssertFail(#X_CONDITION, "Assumption failed (debug)",              \
                       __PRETTY_FUNCTION__, __FILE__, #X_LINE);                \
    }                                                                          \
  } while (false)
#define _WSH_ASSUME_1(X_CONDITION, X_LINE) _WSH_ASSUME_2(X_CONDITION, X_LINE)
#define _WSH_ASSUME(X_CONDITION) _WSH_ASSUME_1(X_CONDITION, __LINE__)

#else

#define _WSH_ASSUME(X_CONDITION) [[__assume__]] (X_CONDITION)

#endif

#define _WSH_UNREACHABLE_2(X_LINE)                                             \
  do {                                                                         \
    util::AssertFail(nullptr, "Unreachable code executed",                     \
                     __PRETTY_FUNCTION__, __FILE__, #X_LINE);                  \
  } while (false)
#define _WSH_UNREACHABLE_1(X_LINE) _WSH_UNREACHABLE_2(X_LINE)
#define _WSH_UNREACHABLE() _WSH_UNREACHABLE_1(__LINE__)

} // namespace wsh::util