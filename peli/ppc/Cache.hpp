// peli/ppc/Cache.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "Hid0.hpp"
#include "Sync.hpp"

namespace peli::ppc::Cache {

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

template <Op XOp> inline void OpRange(const void *block, u32 size) noexcept {
  if (size == 0) {
    return;
  }

  const u8 *address = reinterpret_cast<const u8 *>(block);

  for (const u8 *start = address, *end = address + size; start < end;
       start += BlockSize) {
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
}

inline void DcStore(const void *start, u32 size) noexcept {
  OpRange<Op::DcStore>(start, size);
}
inline void DcFlush(const void *start, u32 size) noexcept {
  OpRange<Op::DcFlush>(start, size);
}
inline void DcInvalidate(const void *start, u32 size) noexcept {
  OpRange<Op::DcInvalidate>(start, size);
}
inline void DcZero(const void *start, u32 size) noexcept {
  OpRange<Op::DcZero>(start, size);
}
inline void DcTouch(const void *start, u32 size) noexcept {
  OpRange<Op::DcTouch>(start, size);
}
inline void DcTouchStore(const void *start, u32 size) noexcept {
  OpRange<Op::DcTouchStore>(start, size);
}
inline void IcInvalidate(const void *start, u32 size) noexcept {
  OpRange<Op::IcInvalidate>(start, size);
}

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

} // namespace peli::ppc::Cache