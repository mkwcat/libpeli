// peli/hw/Bit.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../util/Bit.hpp"

namespace peli::hw {

constexpr u32 BitMask(int value) noexcept {
  return util::BitMask<false>(value);
}

constexpr u32 BitMask(int left, int right) noexcept {
  return util::BitMask<false>(left, right);
}

template <class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum value) noexcept {
  return util::BitMask<false>(static_cast<int>(value));
}

template <class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum left, Enum right) noexcept {
  return util::BitMask<false>(static_cast<int>(left), static_cast<int>(right));
}

} // namespace peli::hw
