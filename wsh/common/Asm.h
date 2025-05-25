// wsh/common/Asm.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Types.h"

// TODO: This kind of ASM statement doesn't work with Clang
__asm__(".set sp, 1;");
__asm__(".set toc, 2;");

#define _WSH_ASM_EVAL2(...) __asm__(#__VA_ARGS__)
#define _WSH_ASM_EVAL(...) _WSH_ASM_EVAL2(__VA_ARGS__)

#define WSH_ASM(...) _WSH_ASM_EVAL(__VA_ARGS__)

#define WSH_ASM_FUNCTION(X_PROTOTYPE, ...)                                     \
  _WSH_PRAGMA(GCC diagnostic push)                                             \
  _WSH_PRAGMA(GCC diagnostic ignored "-Wunused-parameter")                     \
  [[__gnu__::__optimize__("Os")]] X_PROTOTYPE {                                \
    WSH_ASM(__VA_ARGS__);                                                      \
    __builtin_unreachable();                                                   \
  }                                                                            \
  _WSH_PRAGMA(GCC diagnostic pop)
