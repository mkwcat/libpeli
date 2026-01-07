// peli/util/Constructor.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"

namespace peli::util {

/**
 * Utility parameter type to indicate no construction.
 */
struct NoConstruct {};

/**
 * Utility parameter type to indicate in-place construction to operator new.
 * Intended to avoid redefinition of placement new from <new>, without needing
 * to include it.
 */
struct InPlace {
  void *ptr;
};

} // namespace peli::util

constexpr void *operator new(peli::size_t,
                             peli::util::InPlace in_place) noexcept {
  return in_place.ptr;
}

namespace peli::util {

template <class T> constexpr void Construct(T &obj, auto &&...params) noexcept {
  new (InPlace{static_cast<void *>(&obj)})
      T(static_cast<decltype(params)>(params)...);
}

} // namespace peli::util