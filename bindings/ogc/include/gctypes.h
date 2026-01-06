// bindings/ogc/include/gctypes.h
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#if !defined(__GCTYPES_H__)
#define __GCTYPES_H__

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
namespace ogc {
#endif

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#if defined(__cplusplus)
} // namespace ogc
using namespace ogc;
} // extern "C"
#endif

#endif // __GCTYPES_H__