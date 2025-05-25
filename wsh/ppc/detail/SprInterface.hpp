// wsh/ppc/detail/SprInterface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../Spr.hpp"

namespace wsh::ppc::detail {

template <class T, Spr Reg = T::SPR> class SprInterface : public T {
public:
  static constexpr Spr SPR = Reg;

  SprInterface() noexcept = default;
  SprInterface(u32 value) noexcept { Hex() = value; }
  SprInterface(const T &value) noexcept {
    Hex() = *reinterpret_cast<const u32 *>(&value);
  }
  SprInterface(const SprInterface &rhs) noexcept { Hex() = rhs.Hex(); }

  u32 &Hex() noexcept {
    auto *upcast = static_cast<T *>(this);
    return *reinterpret_cast<u32 *>(upcast);
  }

  const u32 &Hex() const noexcept {
    auto *upcast = static_cast<const T *>(this);
    return *reinterpret_cast<const u32 *>(upcast);
  }

  [[nodiscard]]
  static SprInterface MoveFrom() noexcept {
    return SprInterface(GetSpr<SPR>());
  }

  void MoveTo() const noexcept { SetSpr<SPR>(Hex()); }
  static void MoveTo(u32 value) noexcept { SetSpr<SPR>(value); }
  static void MoveTo(const T &value) noexcept {
    MoveTo(SprInterface(value).Hex());
  }

  u32 operator*() const noexcept { return Hex(); }

  // Arithmetic operators

  SprInterface operator+(u32 rhs) const noexcept {
    return SprInterface(Hex() + rhs);
  }
  SprInterface operator-(u32 rhs) const noexcept {
    return SprInterface(Hex() - rhs);
  }
  SprInterface operator*(u32 rhs) const noexcept {
    return SprInterface(Hex() * rhs);
  }
  SprInterface operator/(u32 rhs) const noexcept {
    return SprInterface(Hex() / rhs);
  }
  SprInterface operator%(u32 rhs) const noexcept {
    return SprInterface(Hex() % rhs);
  }

  SprInterface operator+(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() + rhs.Hex());
  }
  SprInterface operator-(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() - rhs.Hex());
  }
  SprInterface operator*(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() * rhs.Hex());
  }
  SprInterface operator/(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() / rhs.Hex());
  }
  SprInterface operator%(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() % rhs.Hex());
  }

  SprInterface operator+(const T &rhs) const noexcept {
    return SprInterface(Hex() + SprInterface(rhs).Hex());
  }
  SprInterface operator-(const T &rhs) const noexcept {
    return SprInterface(Hex() - SprInterface(rhs).Hex());
  }
  SprInterface operator*(const T &rhs) const noexcept {
    return SprInterface(Hex() * SprInterface(rhs).Hex());
  }
  SprInterface operator/(const T &rhs) const noexcept {
    return SprInterface(Hex() / SprInterface(rhs).Hex());
  }
  SprInterface operator%(const T &rhs) const noexcept {
    return SprInterface(Hex() % SprInterface(rhs).Hex());
  }

  // Bitwise operators

  SprInterface operator|(u32 rhs) const noexcept {
    return SprInterface(Hex() | rhs);
  }
  SprInterface operator&(u32 rhs) const noexcept {
    return SprInterface(Hex() & rhs);
  }
  SprInterface operator^(u32 rhs) const noexcept {
    return SprInterface(Hex() ^ rhs);
  }
  SprInterface operator~() const noexcept { return SprInterface(~Hex()); }
  SprInterface operator<<(u32 rhs) const noexcept {
    return SprInterface(Hex() << rhs);
  }
  SprInterface operator>>(u32 rhs) const noexcept {
    return SprInterface(Hex() >> rhs);
  }

  SprInterface operator|(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() | rhs.Hex());
  }
  SprInterface operator&(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() & rhs.Hex());
  }
  SprInterface operator^(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() ^ rhs.Hex());
  }
  SprInterface operator<<(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() << rhs.Hex());
  }
  SprInterface operator>>(const SprInterface &rhs) const noexcept {
    return SprInterface(Hex() >> rhs.Hex());
  }

  SprInterface operator|(const T &rhs) const noexcept {
    return SprInterface(Hex() | SprInterface(rhs).Hex());
  }
  SprInterface operator&(const T &rhs) const noexcept {
    return SprInterface(Hex() & SprInterface(rhs).Hex());
  }
  SprInterface operator^(const T &rhs) const noexcept {
    return SprInterface(Hex() ^ SprInterface(rhs).Hex());
  }
  SprInterface operator<<(const T &rhs) const noexcept {
    return SprInterface(Hex() << SprInterface(rhs).Hex());
  }
  SprInterface operator>>(const T &rhs) const noexcept {
    return SprInterface(Hex() >> SprInterface(rhs).Hex());
  }

  // Comparison operators

  bool operator==(u32 rhs) const noexcept { return Hex() == rhs; }
  bool operator!=(u32 rhs) const noexcept { return Hex() != rhs; }
  bool operator<(u32 rhs) const noexcept { return Hex() < rhs; }
  bool operator<=(u32 rhs) const noexcept { return Hex() <= rhs; }
  bool operator>(u32 rhs) const noexcept { return Hex() > rhs; }
  bool operator>=(u32 rhs) const noexcept { return Hex() >= rhs; }

  bool operator==(const SprInterface &rhs) const noexcept {
    return Hex() == rhs.Hex();
  }
  bool operator!=(const SprInterface &rhs) const noexcept {
    return Hex() != rhs.Hex();
  }
  bool operator<(const SprInterface &rhs) const noexcept {
    return Hex() < rhs.Hex();
  }
  bool operator<=(const SprInterface &rhs) const noexcept {
    return Hex() <= rhs.Hex();
  }
  bool operator>(const SprInterface &rhs) const noexcept {
    return Hex() > rhs.Hex();
  }
  bool operator>=(const SprInterface &rhs) const noexcept {
    return Hex() >= rhs.Hex();
  }

  bool operator==(const T &rhs) const noexcept {
    return Hex() == SprInterface(rhs).Hex();
  }
  bool operator!=(const T &rhs) const noexcept {
    return Hex() != SprInterface(rhs).Hex();
  }
  bool operator<(const T &rhs) const noexcept {
    return Hex() < SprInterface(rhs).Hex();
  }
  bool operator<=(const T &rhs) const {
    return Hex() <= SprInterface(rhs).Hex();
  }
  bool operator>(const T &rhs) const noexcept {
    return Hex() > SprInterface(rhs).Hex();
  }
  bool operator>=(const T &rhs) const {
    return Hex() >= SprInterface(rhs).Hex();
  }

  // Assignment operators

  SprInterface &operator=(u32 rhs) noexcept {
    Hex() = rhs;
    return *this;
  }
  SprInterface &operator=(const SprInterface &rhs) noexcept {
    *this = rhs.Hex();
    return *this;
  }
  SprInterface &operator=(const T &rhs) noexcept {
    *this = SprInterface(rhs).Hex();
    return *this;
  }

  SprInterface &operator+=(const auto &rhs) noexcept {
    return *this = *this + rhs;
  }
  SprInterface &operator-=(const auto &rhs) noexcept {
    return *this = *this - rhs;
  }
  SprInterface &operator*=(const auto &rhs) noexcept {
    return *this = *this * rhs;
  }
  SprInterface &operator/=(const auto &rhs) noexcept {
    return *this = *this / rhs;
  }
  SprInterface &operator%=(const auto &rhs) noexcept {
    return *this = *this % rhs;
  }
  SprInterface &operator|=(const auto &rhs) noexcept {
    return *this = *this | rhs;
  }
  SprInterface &operator&=(const auto &rhs) noexcept {
    return *this = *this & rhs;
  }
  SprInterface &operator^=(const auto &rhs) noexcept {
    return *this = *this ^ rhs;
  }
  SprInterface &operator<<=(const auto &rhs) noexcept {
    return *this = *this << rhs;
  }
  SprInterface &operator>>=(const auto &rhs) noexcept {
    return *this = *this >> rhs;
  }

  SprInterface &Clear(u32 rhs) noexcept {
    *this &= ~rhs;
    return *this;
  }
  SprInterface &Clear(const SprInterface &rhs) noexcept {
    *this &= ~rhs;
    return *this;
  }
  SprInterface &Clear(const T &rhs) noexcept {
    *this &= ~SprInterface(rhs);
    return *this;
  }
};

template <class T>
concept SprInterfaceConcept = requires(T t) {
  T::MoveFrom();
  T().MoveTo();
};

} // namespace wsh::ppc::detail