// peli/common/Asm.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Macro.h"

#define _PELI_ASM_EVAL_ARGS2(...) __VA_ARGS__
#define _PELI_ASM_EVAL_ARGS(...) _PELI_ASM_EVAL_ARGS2(__VA_ARGS__)

#define PELI_ASM(...) (_PELI_STRINGIFY(__VA_ARGS__))

#define PELI_ASM_EXTENDED(X_ARGS, ...)                                         \
  (_PELI_STRINGIFY(__VA_ARGS__) : _PELI_ASM_EVAL_ARGS(                         \
      _PELI_ASM_EVAL_ARGS2 X_ARGS))

#define PELI_ASM_METHOD(X_PROTOTYPE, X_ARGS, ...)                              \
  _PELI_DIAGNOSTIC(push)                                                       \
  _PELI_DIAGNOSTIC(ignored "-Wunused-parameter")                               \
  _PELI_DIAGNOSTIC_CLANG(ignored "-Wunknown-attributes")                       \
  [[__gnu__::__optimize__("Os")]] X_PROTOTYPE {                                \
    __asm__(_PELI_STRINGIFY(__VA_ARGS__)::_PELI_ASM_EVAL_ARGS(                 \
        _PELI_ASM_EVAL_ARGS2 X_ARGS));                                         \
    __builtin_unreachable();                                                   \
  }                                                                            \
  _PELI_DIAGNOSTIC(pop)

#define PELI_ASM_IMPORT(X_CONSTRAINT, X_SYMBOL)                                \
  [X_SYMBOL] #X_CONSTRAINT(X_SYMBOL)
