// peli/cmn/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli {

namespace cmn {

using size_t = decltype(sizeof(0));

/**
 * 8-bit unsigned integer.
 */
#if defined(__UINT8_TYPE__)
using u8 = __UINT8_TYPE__;
#else
using u8 = unsigned char;
#endif

/**
 * 16-bit unsigned integer.
 */
#if defined(__UINT16_TYPE__)
using u16 = __UINT16_TYPE__;
#else
using u16 = unsigned short;
#endif

/**
 * 32-bit unsigned integer.
 */
#if defined(__UINT32_TYPE__)
using u32 = __UINT32_TYPE__;
#else
using u32 = unsigned int;
#endif

/**
 * 64-bit unsigned integer.
 */
#if defined(__UINT64_TYPE__)
using u64 = __UINT64_TYPE__;
#else
using u64 = long long unsigned int;
#endif

/**
 * 8-bit signed integer.
 */
#if defined(__INT8_TYPE__)
using s8 = __INT8_TYPE__;
#else
using s8 = signed char;
#endif

/**
 * 16-bit signed integer.
 */
#if defined(__INT16_TYPE__)
using s16 = __INT16_TYPE__;
#else
using s16 = short;
#endif

/**
 * 32-bit signed integer.
 */
#if defined(__INT32_TYPE__)
using s32 = __INT32_TYPE__;
#else
using s32 = int;
#endif

/**
 * 64-bit signed integer.
 */
#if defined(__INT64_TYPE__)
using s64 = __INT64_TYPE__;
#else
using s64 = long long int;
#endif

/**
 * 32-bit floating point number.
 */
using f32 = float;

/**
 * 64-bit floating point number.
 */
using f64 = double;

static_assert(sizeof(u8) == 1 && sizeof(s8) == 1,
              "u8/s8 must be exactly 8 bits");
static_assert(sizeof(u16) == 2 && sizeof(s16) == 2,
              "u16/s16 must be exactly 16 bits");
static_assert(sizeof(u32) == 4 && sizeof(s32) == 4,
              "u32/s32 must be exactly 32 bits");
static_assert(sizeof(u64) == 8 && sizeof(s64) == 8,
              "u64/s64 must be exactly 64 bits");
static_assert(sizeof(f32) == 4, "f32 must be exactly 32 bits");
static_assert(sizeof(f64) == 8, "f64 must be exactly 64 bits");

} // namespace cmn

// Import types into peli namespace
using namespace cmn;

} // namespace peli
