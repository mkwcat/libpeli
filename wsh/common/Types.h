// wsh/common/Types.h
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#if !defined(__cplusplus)
#include <stdbool.h>
#include <stdint.h>
#else
#include <cstdint>
#endif // __cplusplus

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

#define _WSH_PACKED __attribute__((__packed__))
#define _WSH_ALIGN(v) __attribute__((__aligned__(v)))
#define _WSH_UNREACHABLE __builtin_unreachable()
#define _WSH_PRAGMA(x) _Pragma(#x)
