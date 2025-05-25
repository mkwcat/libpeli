// wsh/util/CpuCache.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../ppc/Cache.hpp"

namespace wsh::util {

class CpuCache {
public:
  static void DcFlush(const void *addr, u32 size) noexcept {
    ppc::Cache::DcFlush(addr, size);
  }

  template <class T> static void DcFlush(const T &data) noexcept {
    ppc::Cache::DcFlush(&data, sizeof(T));
  }

  static void DcInvalidate(const void *addr, u32 size) noexcept {
    ppc::Cache::DcInvalidate(addr, size);
  }

  template <class T> static void DcInvalidate(const T &data) noexcept {
    ppc::Cache::DcInvalidate(&data, sizeof(T));
  }
};

} // namespace wsh::util