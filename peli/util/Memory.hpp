// peli/util/Memory.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../host/Host.hpp"
#include "Address.hpp"
#include "Concept.hpp"

namespace peli::util {

template <bool BigEndian = host::IsBigEndian, IntegralType T> T SwapTo(T v) {
  if (BigEndian != host::IsBigEndian) {
    if constexpr (sizeof(T) == 2) {
      v = __builtin_bswap16(v);
    } else if constexpr (sizeof(T) == 4) {
      v = __builtin_bswap32(v);
    } else if constexpr (sizeof(T) == 8) {
      v = __builtin_bswap64(v);
    }
  }
  return v;
}

template <class T, bool BigEndian = host::IsBigEndian>
T ImmRead(MemoryAddressType auto const ptr, ::size_t offset = 0) noexcept {
  return SwapTo<BigEndian>(
      *reinterpret_cast<const T *>(reinterpret_cast<const u8 *>(ptr) + offset));
}

template <class T, bool BigEndian = host::IsBigEndian>
void ImmWrite(MemoryAddressType auto const ptr, T value) noexcept {
  *reinterpret_cast<T *>(ptr) = SwapTo<BigEndian>(value);
}

template <class T, bool BigEndian = host::IsBigEndian>
void ImmWrite(MemoryAddressType auto const ptr, ::size_t offset,
              T value) noexcept {
  *reinterpret_cast<T *>(reinterpret_cast<u8 *>(ptr) + offset) =
      SwapTo<BigEndian>(value);
}

template <class T, bool BigEndian = host::IsBigEndian>
T ImmReadMisaligned(MemoryAddressType auto const ptr,
                    ::size_t offset = 0) noexcept {
  T value = 0;
  const u8 *p = reinterpret_cast<const u8 *>(ptr) + offset;
  for (size_t i = 0; i < sizeof(T); i++) {
    if constexpr (BigEndian) {
      value = (value << 8) | p[i];
    } else {
      value |= (T)p[i] << (i * 8);
    }
  }
  return value;
}

} // namespace peli::util