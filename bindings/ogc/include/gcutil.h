// bindings/ogc/include/gcutil.h
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#if !defined(__GCUTIL_H__)
#define __GCUTIL_H__

#if !defined(ATTRIBUTE_ALIGN)

#if defined(__GNUC__) || (defined(__has_attribute) && __has_attribute(aligned))
#define ATTRIBUTE_ALIGN(x) __attribute__((__aligned__(x)))

#elif defined(__has_cpp_attribute) && __has_cpp_attribute(__gnu__::__aligned__)
#define ATTRIBUTE_ALIGN(x) [[__gnu__::__aligned__(x)]]

#elif defined(_MSC_VER) || defined(__METROWERKS__)
#define ATTRIBUTE_ALIGN(x) __declspec(align(x))

#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L &&              \
    __STDC_VERSION__ < 202311L
#define ATTRIBUTE_ALIGN(x) _Alignas(x)

#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) ||            \
    (defined(__cplusplus) && __cplusplus >= 201103L)
#define ATTRIBUTE_ALIGN(x) alignas(x)
#endif

#endif // !defined(ATTRIBUTE_ALIGN)

#if !defined(ATTRIBUTE_PACKED)

#if defined(__GNUC__) ||                                                       \
    (defined(__has_attribute) && __has_attribute(__packed__))
#define ATTRIBUTE_PACKED(x) __attribute__((__packed__(x)))

#elif defined(__has_cpp_attribute) && __has_cpp_attribute(__gnu__::__packed__)
#define ATTRIBUTE_PACKED(x) [[__gnu__::__packed__(x)]]
#endif

#endif // !defined(ATTRIBUTE_PACKED)

#endif // __GCUTIL_H__