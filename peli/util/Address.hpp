// peli/util/Address.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Macro.h"
#include "../cmn/Types.hpp"

namespace peli::util {

_PELI_DIAGNOSTIC(push)
_PELI_DIAGNOSTIC(ignored "-Wold-style-cast")

template <class T> constexpr T AlignUp(size_t align, T num) noexcept {
  size_t raw = (size_t)num;
  raw += align - 1;
  return (T)(raw - (raw % align));
}

template <class T> constexpr T AlignDown(size_t align, T num) noexcept {
  size_t raw = (size_t)num;
  return (T)(raw - (raw % align));
}

template <class T> constexpr bool IsAligned(size_t align, T num) noexcept {
  size_t raw = (size_t)num;
  return (raw % align) == 0;
}

template <class T1, class T2>
constexpr bool CheckBounds(T1 bounds, size_t boundLen, T2 buffer,
                           size_t len) noexcept {
  size_t low = (size_t)bounds;
  size_t high = low + boundLen;
  size_t inside = (size_t)buffer;
  size_t insidehi = inside + len;

  return high >= low && insidehi >= inside && inside >= low && insidehi <= high;
}

template <class T> constexpr bool InMEM1(T addr) noexcept {
  const u32 value = (u32)addr;
  return value < 0x01800000;
}

template <class T> constexpr bool InMEM2(T addr) noexcept {
  const u32 value = (u32)addr;
  return (value >= 0x10000000) && (value < 0x14000000);
}

template <class T> constexpr bool InMEM1Effective(T addr) noexcept {
  const u32 value = (u32)addr;
  return (value >= 0x80000000) && (value < 0x81800000);
}

template <class T> constexpr bool InMEM2Effective(T addr) noexcept {
  const u32 value = (u32)addr;
  return (value >= 0x90000000) && (value < 0x94000000);
}

template <class T> constexpr T Effective(T addr) noexcept {
  return (T)(((u32)addr & ~0xC0000000) | 0x80000000);
}

template <class T> constexpr T Physical(T addr) noexcept {
  return (T)((u32)addr & ~0xC0000000);
}

template <class T> constexpr T Uncached(T addr) noexcept {
  return (T)((u32)addr | 0xC0000000);
}

_PELI_DIAGNOSTIC(pop)

} // namespace peli::util