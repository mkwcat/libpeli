// peli/util/String.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"
#include "Optimize.hpp"

namespace peli::util {

constexpr size_t StrLen(const char *s) {
#if defined(__GNUC__)
  return ::__builtin_strlen(s);
#else
  const char *f = s;
  while (*s != '\0') {
    s++;
  }
  return s - f;
#endif
}

template <size_t N> consteval size_t StrLen(const char (&)[N]) { return N - 1; }

template <size_t TMax = -1>
constexpr size_t StrCopy(char *_PELI_RESTRICT dst,
                         const char *_PELI_RESTRICT src) {
  if constexpr (TMax == 0) {
    return 0;
  }

  for (size_t i = 0; i < TMax; i++) {
    dst[i] = src[i];
    if (dst[i] == 0) {
      return i;
    }
  }

  dst[TMax - 1] = 0;
  return TMax;
}

} // namespace peli::util