// peli/hw/Register.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::hw {

template <class T> class Register : public T {
public:
  using SizeType = T::Size;

  SizeType Hex() {
    static_assert(sizeof(*this) == sizeof(SizeType));
    volatile auto *upcast = static_cast<T *>(this);
    return *reinterpret_cast<volatile SizeType *>(upcast);
  }

  const SizeType Hex() const {
    volatile auto *upcast = static_cast<const T *>(this);
    return *reinterpret_cast<volatile const SizeType *>(upcast);
  }

  Register() = default;
  Register(SizeType value) { operator=(value); }
  Register(const T &value)
      : Register(*reinterpret_cast<const SizeType *>(&value)) {}
  Register(const Register &rhs) : Register(rhs.Hex()) {}

  Register &operator=(const Register &rhs) { return operator=(rhs.Hex()); }
  Register &operator=(const T &rhs) {
    return operator=(*reinterpret_cast<const SizeType *>(&rhs));
  }
  Register &operator=(SizeType rhs) {
    volatile auto *upcast = static_cast<T *>(this);
    *reinterpret_cast<volatile SizeType *>(upcast) = rhs;
    return *this;
  }

  SizeType operator*() const { return Hex(); }
  operator SizeType() const { return Hex(); }

  // Sub-expression
  constexpr void operator<=>(const auto &func) {
    *this = func(*static_cast<T *>(this));
  }

  constexpr void operator<=(const auto &func) { operator=(func()); }

  Register &operator|=(const Register &rhs) {
    return operator=(Hex() | rhs.Hex());
  }
  Register &operator&=(const Register &rhs) {
    return operator=(Hex() & rhs.Hex());
  }
  Register &operator^=(const Register &rhs) {
    return operator=(Hex() ^ rhs.Hex());
  }
  Register &operator|=(SizeType rhs) { return operator=(Hex() | rhs); }
  Register &operator&=(SizeType rhs) { return operator=(Hex() & rhs); }
  Register &operator^=(SizeType rhs) { return operator=(Hex() ^ rhs); }
};

} // namespace peli::hw