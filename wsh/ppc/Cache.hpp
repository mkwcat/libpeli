// wsh/ppc/Cache.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../util/Address.hpp"
#include "Hid0.hpp"
#include "Sync.hpp"

namespace wsh::ppc::Cache {

constexpr u32 BlockSize = 32;

enum class Op {
  DcStore,
  DcFlush,
  DcInvalidate,
  DcZero,
  DcTouch,
  DcTouchStore,
  IcInvalidate,
};

template <Op XOp, bool DoSync, bool AssumeAligned = false>
inline void OpRange(const void *block, u32 size) noexcept {
  if (size == 0) {
    return;
  }

  const u8 *address = reinterpret_cast<const u8 *>(block);

  if constexpr (AssumeAligned) {
    [[assume(util::AlignDown(BlockSize, address) == address)]];
    [[assume(util::AlignUp(BlockSize, address + size) == address + size)]];
  }

  for (const u8 *start = util::AlignDown(BlockSize, address),
                *end = util::AlignUp(BlockSize, address + size);
       start < end; start += BlockSize) {
    // Perform the cache operation on the block
    if constexpr (XOp == Op::DcStore) {
      asm("dcbst %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::DcFlush) {
      asm("dcbf %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::DcInvalidate) {
      asm("dcbi %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::DcZero) {
      asm("dcbz %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::DcTouch) {
      asm("dcbt %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::DcTouchStore) {
      asm("dcbtst %y0" : : "Z"(*start));
    } else if constexpr (XOp == Op::IcInvalidate) {
      asm("icbi %y0" : : "Z"(*start));
    }
  }

  if constexpr (DoSync) {
    Sync();
    if constexpr (XOp == Op::IcInvalidate) {
      ISync();
    }
  }
}

template <bool DoSync = true>
void DcStore(const void *start, u32 size) noexcept;
template <bool DoSync = true>
void DcFlush(const void *start, u32 size) noexcept;
template <bool DoSync = true>
void DcInvalidate(const void *start, u32 size) noexcept;
template <bool DoSync = true> void DcZero(const void *start, u32 size) noexcept;
template <bool DoSync = true>
void DcTouch(const void *start, u32 size) noexcept;
template <bool DoSync = true>
void DcTouchStore(const void *start, u32 size) noexcept;
template <bool DoSync = true>
void IcInvalidate(const void *start, u32 size) noexcept;

inline void DcEnable(bool enable) noexcept { SprRwCtl<Hid0>()->DCE = enable; }

inline void IcEnable(bool enable) noexcept { SprRwCtl<Hid0>()->ICE = enable; }

inline void DcLock(bool lock) noexcept { SprRwCtl<Hid0>()->DLOCK = lock; }

inline void IcLock(bool lock) noexcept { SprRwCtl<Hid0>()->ILOCK = lock; }

inline void DcFlashInvalidate() noexcept {
  SprRwCtl<Hid0>()->DCFI = true;
  Sync();
}

inline void IcFlashInvalidate() noexcept {
  SprRwCtl<Hid0>()->ICFI = true;
  ISync();
  Sync();
}

} // namespace wsh::ppc::Cache