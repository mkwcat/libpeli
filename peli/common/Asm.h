// peli/common/Asm.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Macro.h"

#define PELI_ASM(...) __asm__(_PELI_STRINGIFY(__VA_ARGS__))

#define PELI_ASM_FUNCTION(X_PROTOTYPE, ...)                                    \
  _PELI_DIAGNOSTIC(push)                                                       \
  _PELI_DIAGNOSTIC(ignored "-Wunused-parameter")                               \
  _PELI_DIAGNOSTIC_CLANG(ignored "-Wunknown-attributes")                       \
  [[__gnu__::__optimize__("Os")]] X_PROTOTYPE {                                \
    PELI_ASM(__VA_ARGS__);                                                     \
    __builtin_unreachable();                                                   \
  }                                                                            \
  _PELI_DIAGNOSTIC(pop)
