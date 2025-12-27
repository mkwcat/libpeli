// peli/util/Transform.hpp - Type transformations
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::util {

template <class O> struct Transform {
  Transform() = delete;

  using T = O;

private:
  template <class T> struct identity {
    using X = T;
  };

  template <class T> struct remR : identity<T> {};
  template <class T> struct remR<T &> : identity<T> {};
  template <class T> struct remR<T &&> : identity<T> {};
  template <class T> struct remC : identity<T> {};
  template <class T> struct remC<const T> : identity<T> {};
  template <class T> struct remV : identity<T> {};
  template <class T> struct remV<volatile T> : identity<T> {};

public:
  /**
   * Returns the same type. Useful for removing type deduction.
   */
  using Identity = typename identity<T>::X;


  /**
   * Remove reference from type.
   */
  using RemR = Transform<typename remR<T>::X>;

  /**
   * Remove const from type.
   */
  using RemC = Transform<typename remC<T>::X>;

  /**
   * Remove volatile from type.
   */
  using RemV = Transform<typename remV<T>::X>;

  /**
   * Remove const volatile reference from type.
   */
  using RemCVR = RemC::RemV::RemR;
};

} // namespace peli::util