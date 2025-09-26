// peli/common/Types.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)

namespace peli {

// Value type definitions

/**
 * 8-bit unsigned integer.
 */
typedef uint8_t u8;

/**
 * 16-bit unsigned integer.
 */
typedef uint16_t u16;

/**
 * 32-bit unsigned integer.
 */
typedef uint32_t u32;

/**
 * 64-bit unsigned integer.
 */
typedef uint64_t u64;

/**
 * 8-bit signed integer.
 */
typedef int8_t s8;

/**
 * 16-bit signed integer.
 */
typedef int16_t s16;

/**
 * 32-bit signed integer.
 */
typedef int32_t s32;

/**
 * 64-bit signed integer.
 */
typedef int64_t s64;

/**
 * 32-bit floating point number.
 */
typedef float f32;

/**
 * 64-bit floating point number.
 */
typedef double f64;

} // namespace peli

#endif // __cplusplus

// Some type-related preprocessor definitions

#define _PELI_PAD(X_START, X_END) char _Pad##X_START[X_END - X_START]
#define _PELI_PACKED __attribute__((__packed__))
#define _PELI_PRAGMA(x) _Pragma(#x)

#define _PELI_STRINGIFY_EXPANDED(...) #__VA_ARGS__
#define _PELI_STRINGIFY(...) _PELI_STRINGIFY_EXPANDED(__VA_ARGS__)

#if defined(__cplusplus)
#define _PELI_SIZE_ASSERT(X_TYPE, X_SIZE)                                      \
  static_assert(sizeof(X_TYPE) == X_SIZE, #X_TYPE " size mismatch")
#endif

#if defined(__clang__)
#define _PELI_GNU 1
#define _PELI_CLANG_ONLY(...) __VA_ARGS__
#define _PELI_GNU_ONLY(...)
#elif defined(__GNUC__)
#define _PELI_CLANG 1
#define _PELI_CLANG_ONLY(...)
#define _PELI_GNU_ONLY(...) __VA_ARGS__
#else
#define _PELI_CLANG_ONLY(...)
#define _PELI_GNU_ONLY(...)
#endif

#define _PELI_DIAGNOSTIC(...) _PELI_PRAGMA(GCC diagnostic __VA_ARGS__)
#define _PELI_DIAGNOSTIC_CLANG(...)                                            \
  _PELI_CLANG_ONLY(_PELI_PRAGMA(clang diagnostic __VA_ARGS__))
