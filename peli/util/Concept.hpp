// peli/util/Concept.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::util {

#if !defined(__GNUC__)

namespace detail {
template <class T, class U> struct TestSameAs {
  static constexpr bool value = false;
};
template <class T> struct TestSameAs<T, T> {
  static constexpr bool value = true;
};
} // namespace detail

template <class T, class U>
concept SameAs = detail::TestSameAs<T, U>::value;

#else

template <class T, class U>
concept SameAs = __is_same_as(T, U);

#endif

template <class T, class U>
concept IsConvertibleTo = __is_nothrow_convertible(T, U);

template <class T>
concept IntegralType =
    SameAs<T, bool> || SameAs<T, char> || SameAs<T, signed char> ||
    SameAs<T, unsigned char> || SameAs<T, wchar_t> || SameAs<T, char16_t> ||
    SameAs<T, char32_t> || SameAs<T, short> || SameAs<T, unsigned short> ||
    SameAs<T, int> || SameAs<T, unsigned int> || SameAs<T, long> ||
    SameAs<T, unsigned long> || SameAs<T, long long> ||
    SameAs<T, unsigned long long>
#if defined(__SIZEOF_INT128__)
    || SameAs<T, __int128> || SameAs<T, unsigned __int128>
#endif
    ;

template <class T>
concept PointerType = requires(T t) { static_cast<const void *const>(t); };

template <class T>
concept MemoryAddressType = IntegralType<T> || PointerType<T>;

template <class T>
concept StdArrayType = requires(T t) {
  { t.size() } -> IsConvertibleTo<__SIZE_TYPE__>;
  { t.data() } -> IsConvertibleTo<const void *>;
  typename T::value_type;
};

template <class T>
concept EnumType = __is_enum(T);

template <class T>
struct NoInferStruct {
  using Type = T;
};

template <class T>
using NoInfer = typename NoInferStruct<T>::Type;

} // namespace peli::util