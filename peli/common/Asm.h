// peli/common/Asm.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Types.h"

#define _PELI_ASM_EVAL2(...) __asm__(#__VA_ARGS__)
#define _PELI_ASM_EVAL(...) _PELI_ASM_EVAL2(__VA_ARGS__)

#define PELI_ASM(...) _PELI_ASM_EVAL(__VA_ARGS__)

#define PELI_ASM_FUNCTION(X_PROTOTYPE, ...)                                     \
  _PELI_PRAGMA(GCC diagnostic push)                                             \
  _PELI_PRAGMA(GCC diagnostic ignored "-Wunused-parameter")                     \
  [[__gnu__::__optimize__("Os")]] X_PROTOTYPE {                                \
    PELI_ASM(__VA_ARGS__);                                                      \
    __builtin_unreachable();                                                   \
  }                                                                            \
  _PELI_PRAGMA(GCC diagnostic pop)
