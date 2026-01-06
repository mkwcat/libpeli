// peli/util/Bit.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "Concept.hpp"

namespace peli::util {

template <bool BigEndian = true>
constexpr u32 BitMask(int left, int right) noexcept {
  u32 mask = 0xFFFFFFFFu >> (BigEndian ? left : 31 - left);
  right += 1;
  mask ^= right >= 32 ? 0 : 0xFFFFFFFFu >> (BigEndian ? right : 31 - right);
  if (right < left) {
    mask = ~mask;
  }
  return mask;
}

static_assert(BitMask<true>(0, 0) == 0x80000000u);
static_assert(BitMask<true>(0, 1) == 0xC0000000u);
static_assert(BitMask<true>(30, 32) == 0x00000003u);

template <bool BigEndian = true> constexpr u32 BitMask(int bit) noexcept {
  return 1 << (BigEndian ? 31 - bit : bit);
}

template <bool BigEndian = true>
constexpr u32 BitMask(EnumType auto value) noexcept {
  return BitMask<BigEndian>(static_cast<int>(value));
}

template <bool BigEndian = true>
constexpr u32 BitMask(EnumType auto left, EnumType auto right) noexcept {
  return BitMask<BigEndian>(static_cast<int>(left), static_cast<int>(right));
}

constexpr int CountLeadingZero(IntegralType auto value) {
#if defined(__GNUC__)
  return __builtin_clzg(value);
#else // __GNUC__
  int count = 0;
  constexpr decltype(value) mask = 1 << (sizeof(value) * 8 - 1);
  while ((value & mask) == 0) {
    ++count;
    value <<= 1;
  }
  return count;
#endif
}

template <class T> constexpr T BitCast(auto &&from) {
  return __builtin_bit_cast(T, from);
}

} // namespace peli::util