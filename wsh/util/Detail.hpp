// wsh/util/Detail.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::util {

#define _WSH_PAD(X_START, X_END) u8 _Pad##X_START[X_END - X_START]

#ifndef _WSH_PACKED
#define _WSH_PACKED [[__gnu__::__packed__]]
#endif

#define _WSH_SIZE_ASSERT(X_TYPE, X_SIZE)                                       \
  static_assert(sizeof(X_TYPE) == X_SIZE, #X_TYPE " size mismatch")

} // namespace wsh::util