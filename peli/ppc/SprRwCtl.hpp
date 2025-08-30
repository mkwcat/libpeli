// peli/ppc/SprRwCtl.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "detail/SprInterface.hpp"

namespace peli::ppc {

/**
 * A read-and-write class for special-purpose registers (SPRs). The class moves
 * from the SPR on construct, and then moves the updated value back to the SPR
 * on destruction.
 */
template <detail::SprInterfaceConcept T> class SprRwCtl : public T {
public:
  constexpr T *operator->() noexcept { return this; }

  // Sub-expression
  constexpr void operator<=>(const auto &func) noexcept {
    this->T::operator=(func(*static_cast<T *>(this)));
  }

  constexpr void operator<=(const auto &func) noexcept {
    this->T::operator=(func());
  }

  // Move from the SPR
  SprRwCtl() noexcept : T(T::MoveFrom()) {}

  // Move back to the SPR
  ~SprRwCtl() noexcept { T::MoveTo(); }
};

} // namespace peli::ppc