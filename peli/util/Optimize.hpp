// peli/util/Optimize.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

#if defined(PELI_DEBUG)
#include "Halt.hpp"
#endif

namespace peli::util {

#if defined(PELI_DEBUG)

#define _PELI_ASSUME_2(X_CONDITION, X_LINE)                                     \
  do {                                                                         \
    if (!(X_CONDITION)) {                                                      \
      util::AssertFail(#X_CONDITION, "Assumption failed (debug)",              \
                       __PRETTY_FUNCTION__, __FILE__, #X_LINE);                \
    }                                                                          \
  } while (false)
#define _PELI_ASSUME_1(X_CONDITION, X_LINE) _PELI_ASSUME_2(X_CONDITION, X_LINE)
#define _PELI_ASSUME(X_CONDITION) _PELI_ASSUME_1(X_CONDITION, __LINE__)

#else

#define _PELI_ASSUME(X_CONDITION) [[__assume__]] (X_CONDITION)

#endif

#define _PELI_UNREACHABLE_2(X_LINE)                                             \
  do {                                                                         \
    util::AssertFail(nullptr, "Unreachable code executed",                     \
                     __PRETTY_FUNCTION__, __FILE__, #X_LINE);                  \
  } while (false)
#define _PELI_UNREACHABLE_1(X_LINE) _PELI_UNREACHABLE_2(X_LINE)
#define _PELI_UNREACHABLE() _PELI_UNREACHABLE_1(__LINE__)

} // namespace peli::util