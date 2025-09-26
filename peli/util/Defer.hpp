// peli/util/Defer.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::util {

template <class T>
concept Callable = requires(T a) { a(); };

template <Callable TFunc> class Defer {
public:
  explicit Defer(TFunc &&func) noexcept
      : m_func(static_cast<TFunc &&>(func)), m_active(true) {}
  Defer(Defer &&other) noexcept
      : m_func(static_cast<TFunc &&>(other.m_func)), m_active(other.m_active) {
    other.m_active = false;
  }
  ~Defer() noexcept {
    if (m_active) {
      m_func();
    }
  }

  void Cancel() noexcept { m_active = false; }

  Defer(const Defer &) = delete;
  Defer &operator=(const Defer &) = delete;
  Defer &operator=(Defer &&) = delete;

private:
  TFunc m_func;
  bool m_active;
};

} // namespace peli::util