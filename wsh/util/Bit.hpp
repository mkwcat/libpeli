// wsh/util/Bit.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"

namespace wsh::util {

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

template <bool BigEndian = true, class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum value) noexcept {
  return BitMask<BigEndian>(static_cast<int>(value));
}

template <bool BigEndian = true, class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum left, Enum right) noexcept {
  return BitMask<BigEndian>(static_cast<int>(left), static_cast<int>(right));
}

} // namespace wsh::util