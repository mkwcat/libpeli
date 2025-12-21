// peli/host/Host.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"
#include "Config.h"

namespace peli::host {

enum class Endian : u8 {
  Little = 0b00011011,
  Big = 0b11100100,
#if defined(__BYTE_ORDER__)
  Native = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? Little : Big,
#elif defined(_MSC_VER)
  Native = Little,
#else
  Native = Big,
#endif
};

namespace detail {
extern "C" {
void *aligned_alloc(size_t alignment, size_t size);
void free(void *ptr) noexcept;
}
} // namespace detail

inline void *Alloc(size_t alignment, size_t size) {
#if defined(PELI_FUNC_ALLOC)
  return PELI_FUNC_ALLOC(alignment, size);
#else
  return detail::aligned_alloc(alignment, size);
#endif
}

inline void Free(void *ptr, [[maybe_unused]] size_t size) {
#if defined(PELI_FUNC_FREE)
  return PELI_FUNC_FREE(ptr, alignment);
#else
  return detail::free(ptr);
#endif
}

} // namespace peli::host