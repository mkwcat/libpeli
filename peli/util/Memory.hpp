// peli/util/Memory.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "../host/Host.hpp"
#include "Concept.hpp"
#include "Transform.hpp"

namespace peli::util {

template <host::Endian TEndian = host::Endian::Native, class T> T SwapTo(T v) {
  if constexpr (TEndian == host::Endian::Native || sizeof(T) <= 1) {
    return v;
  }

#if defined(__GNUC__)
  if constexpr (IntegralType<T>) {
    if constexpr (sizeof(T) == 2) {
      return static_cast<T>(__builtin_bswap16(static_cast<u16>(v)));
    } else if constexpr (sizeof(T) == 4) {
      return static_cast<T>(__builtin_bswap16(static_cast<u32>(v)));
    } else if constexpr (sizeof(T) == 8) {
      return static_cast<T>(__builtin_bswap16(static_cast<u64>(v)));
    }
  }
#endif

  T out;
  for (size_t i = 0; i < sizeof(T) >> 1; i++) {
    size_t lower = i * 8;
    size_t upper = (sizeof(T) * 8 - 8) - lower;
    size_t shift = upper - lower;

    out |= ((v & (T(0xFF) << upper)) >> shift) |
           ((v & (T(0xFF) << lower)) << shift);
  }

  if (sizeof(T) & 1) {
    out = out | (v & (T(0xFF) << ((sizeof(T) >> 1) * 8)));
  }

  return out;
}

template <class T, host::Endian TEndian = host::Endian::Native>
typename Transform<T>::T ImmRead(size_t address, size_t offset = 0) noexcept {
  return SwapTo<TEndian>(*reinterpret_cast<const T *>(address + offset));
}

template <class T, host::Endian TEndian = host::Endian::Native>
typename Transform<T>::T ImmRead(const auto *ptr, size_t offset = 0) {
  return ImmRead<T, TEndian>(
      reinterpret_cast<size_t>(static_cast<const void *>(ptr)), offset);
}

template <class T, host::Endian TEndian = host::Endian::Native>
void ImmWrite(size_t address, const typename Transform<T>::T &value) noexcept {
  *reinterpret_cast<T *>(address) = SwapTo<TEndian>(value);
}

template <class T, host::Endian TEndian = host::Endian::Native>
void ImmWrite(size_t address, size_t offset,
              const typename Transform<T>::T &value) noexcept {
  *reinterpret_cast<T *>(address + offset) = SwapTo<TEndian>(value);
}

template <class T, host::Endian TEndian = host::Endian::Native>
void ImmWrite(const auto *ptr, const typename Transform<T>::T &value) {
  return ImmWrite<T, TEndian>(
      reinterpret_cast<size_t>(static_cast<const void *>(ptr)), value);
}

template <class T, host::Endian TEndian = host::Endian::Native>
void ImmWrite(const auto *ptr, size_t offset,
              const typename Transform<T>::T &value) {
  return ImmWrite<T, TEndian>(
      reinterpret_cast<size_t>(static_cast<const void *>(ptr)) + offset, value);
}

template <class T, host::Endian TEndian = host::Endian::Native>
typename Transform<T>::T ImmReadMisaligned(MemoryAddressType auto const ptr,
                                           size_t offset = 0) noexcept {
#if defined(__powerpc__)
  return ImmRead<T, TEndian>(ptr, offset);
#else
  T value = 0;
  const u8 *p = reinterpret_cast<const u8 *>(ptr) + offset;
  for (size_t i = 0; i < sizeof(T); i++) {
    if constexpr (TEndian == host::Endian::Big) {
      value = (value << 8) | p[i];
    } else {
      value |= (T)p[i] << (i * 8);
    }
  }
  return value;
#endif
}

constexpr bool MemoryEqual(const void *m1, const void *m2, size_t size) {
#if defined(__GNUC__)
  return ::__builtin_memcmp(m1, m2, size) == 0;
#else
  size_t i;
  for (i = 0; i + sizeof(long) <= size; i += sizeof(long)) {
    if (ImmReadMisaligned<long>(m1, i) != ImmReadMisaligned<long>(m2, i)) {
      return false;
    }
  }

  for (; i < size; i++) {
    if (ImmRead<u8>(m1, i) != ImmRead<u8>(m2, i)) {
      return false;
    }
  }

  return true;
#endif
}

} // namespace peli::util