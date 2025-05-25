// wsh/hw/Bit.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../util/Bit.hpp"

namespace wsh::hw {

constexpr u32 BitMask(u32 value) noexcept {
  return util::BitMask<false>(value);
}

constexpr u32 BitMask(u32 left, u32 right) noexcept {
  return util::BitMask<false>(left, right);
}

template <class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum value) noexcept {
  return util::BitMask<false>(static_cast<u32>(value));
}

template <class Enum>
  requires(__is_enum(Enum))
constexpr u32 BitMask(Enum left, Enum right) noexcept {
  return util::BitMask<false>(static_cast<u32>(left), static_cast<u32>(right));
}

} // namespace wsh::hw