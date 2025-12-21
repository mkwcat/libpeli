// peli/util/CpuCache.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"
#include "../host/Config.h"

#if defined(PELI_HOST_PPC)
#include "../ppc/Cache.hpp"
#include "../ppc/Sync.hpp"
#endif

namespace peli::util {

class CpuCache {
public:
  static void DcFlush(const void *addr, u32 size) noexcept {
    ppc::Cache::DcFlush(addr, size);
#if defined(PELI_HOST_PPC)
    ppc::SyncBroadcast();
#endif
  }

  template <class T> static void DcFlush(const T &data) noexcept {
    ppc::Cache::DcFlush(&data, sizeof(T));
#if defined(PELI_HOST_PPC)
    ppc::SyncBroadcast();
#endif
  }

  static void DcInvalidate(const void *addr, u32 size) noexcept {
    ppc::Cache::DcInvalidate(addr, size);
#if defined(PELI_HOST_PPC)
    ppc::SyncBroadcast();
#endif
  }

  template <class T> static void DcInvalidate(const T &data) noexcept {
    ppc::Cache::DcInvalidate(&data, sizeof(T));
#if defined(PELI_HOST_PPC)
    ppc::SyncBroadcast();
#endif
  }
};

} // namespace peli::util