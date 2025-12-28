#pragma once

#define _PELI_PAD(X_START, X_END) unsigned char _Pad##X_START[X_END - X_START]
#define _PELI_PRAGMA(x) _Pragma(#x)
#define _PELI_PACKED [[__gnu__::__packed__]]
#define _PELI_WEAK [[__gnu__::__weak__]]

#define _PELI_STRINGIFY_EXPANDED(...) #__VA_ARGS__
#define _PELI_STRINGIFY(...) _PELI_STRINGIFY_EXPANDED(__VA_ARGS__)

#define _PELI_CONCAT_EXPANDED(X, ...) X##__VA_ARGS__
#define _PELI_CONCAT(X, ...) _PELI_CONCAT_EXPANDED(X, __VA_ARGS__)

#if defined(__cplusplus)
#define _PELI_SIZE_ASSERT(X_TYPE, X_SIZE)                                      \
  static_assert(sizeof(X_TYPE) == X_SIZE, #X_TYPE " size mismatch")
#endif

#if defined(__clang__)
#define _PELI_CLANG
#define _PELI_GNU_CLANG
#define _PELI_CLANG_ONLY(...) __VA_ARGS__
#define _PELI_GNU_ONLY(...)
#define _PELI_GNU_CLANG_ONLY(...) __VA_ARGS__
#elif defined(__GNUC__)
#define _PELI_GNU
#define _PELI_GNU_CLANG
#define _PELI_CLANG_ONLY(...)
#define _PELI_GNU_ONLY(...) __VA_ARGS__
#define _PELI_GNU_CLANG_ONLY(...) __VA_ARGS__
#else
#define _PELI_CLANG_ONLY(...)
#define _PELI_GNU_ONLY(...)
#define _PELI_GNU_CLANG_ONLY(...)
#endif

#define _PELI_DIAGNOSTIC(...)                                                  \
  _PELI_GNU_CLANG_ONLY(_PELI_PRAGMA(GCC diagnostic __VA_ARGS__))
#define _PELI_DIAGNOSTIC_CLANG(...)                                            \
  _PELI_CLANG_ONLY(_PELI_PRAGMA(clang diagnostic __VA_ARGS__))
