// peli/ppc/Bat.hpp - Block Address Translation
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "Sync.hpp"
#include "detail/SprInterface.hpp"

namespace peli::ppc {

struct BatBits {
  static constexpr int ADDRESS_SHIFT = 17;
  static constexpr u32 MIN_SIZE = 0x20000;

  struct Up {
    /* 0-14 */ u32 BEPI : 15 = 0;
    /* 15-18 */ u32 RESV15 : 4 = 0;
    /* 19-29 */ u32 BL : 11 = 0;
    /* 30 */ u32 VS : 1 = 0;
    /* 31 */ u32 VP : 1 = 0;
  } up;

  struct Lo {
    /* 0-14 */ u32 BRPN : 15 = 0;
    /* 15-24 */ u32 RESV15 : 10 = 0;
    /* 25-28 */ u32 WIMG : 4 = 0;
    /* 29 */ u32 RESV29 : 1 = 0;
    /* 30-31 */ u32 PP : 2 = 0;
  } lo;
};

template <Spr BatReg> using BatUp = detail::SprInterface<BatBits::Up, BatReg>;
template <Spr BatReg> using BatLo = detail::SprInterface<BatBits::Lo, BatReg>;

using IBat0U = BatUp<Spr::IBAT0U>;
using IBat0L = BatLo<Spr::IBAT0L>;
using IBat1U = BatUp<Spr::IBAT1U>;
using IBat1L = BatLo<Spr::IBAT1L>;
using IBat2U = BatUp<Spr::IBAT2U>;
using IBat2L = BatLo<Spr::IBAT2L>;
using IBat3U = BatUp<Spr::IBAT3U>;
using IBat3L = BatLo<Spr::IBAT3L>;
using IBat4U = BatUp<Spr::IBAT4U>;
using IBat4L = BatLo<Spr::IBAT4L>;
using IBat5U = BatUp<Spr::IBAT5U>;
using IBat5L = BatLo<Spr::IBAT5L>;
using IBat6U = BatUp<Spr::IBAT6U>;
using IBat6L = BatLo<Spr::IBAT6L>;
using IBat7U = BatUp<Spr::IBAT7U>;
using IBat7L = BatLo<Spr::IBAT7L>;
using DBat0U = BatUp<Spr::DBAT0U>;
using DBat0L = BatLo<Spr::DBAT0L>;
using DBat1U = BatUp<Spr::DBAT1U>;
using DBat1L = BatLo<Spr::DBAT1L>;
using DBat2U = BatUp<Spr::DBAT2U>;
using DBat2L = BatLo<Spr::DBAT2L>;
using DBat3U = BatUp<Spr::DBAT3U>;
using DBat3L = BatLo<Spr::DBAT3L>;
using DBat4U = BatUp<Spr::DBAT4U>;
using DBat4L = BatLo<Spr::DBAT4L>;
using DBat5U = BatUp<Spr::DBAT5U>;
using DBat5L = BatLo<Spr::DBAT5L>;
using DBat6U = BatUp<Spr::DBAT6U>;
using DBat6L = BatLo<Spr::DBAT6L>;
using DBat7U = BatUp<Spr::DBAT7U>;
using DBat7L = BatLo<Spr::DBAT7L>;

inline void BatClearAll() {
  IBat0U::MoveTo(0);
  IBat0L::MoveTo(0);
  IBat1U::MoveTo(0);
  IBat1L::MoveTo(0);
  IBat2U::MoveTo(0);
  IBat2L::MoveTo(0);
  IBat3U::MoveTo(0);
  IBat3L::MoveTo(0);
  IBat4U::MoveTo(0);
  IBat4L::MoveTo(0);
  IBat5U::MoveTo(0);
  IBat5L::MoveTo(0);
  IBat6U::MoveTo(0);
  IBat6L::MoveTo(0);
  IBat7U::MoveTo(0);
  IBat7L::MoveTo(0);
  DBat0U::MoveTo(0);
  DBat0L::MoveTo(0);
  DBat1U::MoveTo(0);
  DBat1L::MoveTo(0);
  DBat2U::MoveTo(0);
  DBat2L::MoveTo(0);
  DBat3U::MoveTo(0);
  DBat3L::MoveTo(0);
  DBat4U::MoveTo(0);
  DBat4L::MoveTo(0);
  DBat5U::MoveTo(0);
  DBat5L::MoveTo(0);
  DBat6U::MoveTo(0);
  DBat6L::MoveTo(0);
  DBat7U::MoveTo(0);
  DBat7L::MoveTo(0);
  ISync();
  Sync();
}

/**
 * Configure the BATs (Block Address Translation) to their standard values.
 * Expects to be called in real mode.
 */
inline void BatConfig(u32 mem1Size, u32 mem2Size,
                      [[maybe_unused]] bool lockedCache, bool allowExecInMem2,
                      bool allowExecInUncached) noexcept {
  // Clear old BATs
  BatClearAll();

  const auto bit_floor = [](u32 value) -> u32 {
    if (value == 0) {
      return 0;
    }

    return 1u << (31 - __builtin_clz(value));
  };

  const auto bit_ceil = [](u32 value) -> u32 {
    if (value == 0 || value == 1) {
      return 1;
    }

    return 1u << (32 - __builtin_clz(value - 1));
  };

  // Adjust to the minimum size we can expect, in case the caller expects more
  // than the allocated number of blocks.
  const auto adjust_block_size = [bit_floor](u32 size, int count) -> u32 {
    u32 corrected = 0, lastBit = 0, floored;

    for (int i = 0;
         (floored = bit_floor(size & (lastBit - 1))) >= BatBits::MIN_SIZE &&
         i < count;
         i++) {
      lastBit = floored;
      corrected |= floored;
    }

    if (floored != 0) {
      corrected += lastBit;
    }

    return corrected;
  };

  if (mem1Size >= BatBits::MIN_SIZE) {
    // Create up to 2 blocks for MEM1
    mem1Size = adjust_block_size(mem1Size, 2);
    u32 blockSize = bit_floor(mem1Size);

    // BAT0 - MEM1 Cached
    const BatBits B0Mem1Cached = {
        {
            .BEPI = 0x80000000 >> BatBits::ADDRESS_SHIFT,
            .BL = (blockSize - 1) >> BatBits::ADDRESS_SHIFT,
            .VS = 1,
            .VP = 1,
        },
        {
            .BRPN = 0x00000000 >> BatBits::ADDRESS_SHIFT,
            .WIMG = 0b0000,
            .PP = 2,
        },
    };

    IBat0L::MoveTo(B0Mem1Cached.lo);
    IBat0U::MoveTo(B0Mem1Cached.up);
    ISync();
    DBat0L::MoveTo(B0Mem1Cached.lo);
    DBat0U::MoveTo(B0Mem1Cached.up);
    ISync();
    Sync();

    // BAT1 - MEM1 Uncached and Hardware Interfaces
    // This is always 256 MB
    const BatBits B1Mem1Uncached = {
        {
            .BEPI = 0xC0000000 >> BatBits::ADDRESS_SHIFT,
            .BL = 0x0FFFFFFF >> BatBits::ADDRESS_SHIFT,
            .VS = 1,
            .VP = 1,
        },
        {
            .BRPN = 0x00000000 >> BatBits::ADDRESS_SHIFT,
            .WIMG = 0b0101,
            .PP = 2,
        },
    };

    if (allowExecInUncached) {
      IBat1L::MoveTo(B1Mem1Uncached.lo);
      IBat1U::MoveTo(B1Mem1Uncached.up);
      ISync();
    }
    DBat1L::MoveTo(B1Mem1Uncached.lo);
    DBat1U::MoveTo(B1Mem1Uncached.up);
    ISync();
    Sync();

    // Extend MEM1 to an extra bit of size
    u32 blockAddress = blockSize;
    blockSize = bit_floor(mem1Size & (blockSize - 1));

    if (blockSize >= BatBits::MIN_SIZE) {
      // BAT2 - Extra MEM1 Cached
      const BatBits B2Mem1CachedEx = {
          {
              .BEPI = (0x80000000 + blockAddress) >> BatBits::ADDRESS_SHIFT,
              .BL = (blockSize - 1) >> BatBits::ADDRESS_SHIFT,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = blockAddress >> BatBits::ADDRESS_SHIFT,
              .WIMG = 0b0000,
              .PP = 2,
          },
      };

      IBat2L::MoveTo(B2Mem1CachedEx.lo);
      IBat2U::MoveTo(B2Mem1CachedEx.up);
      ISync();
      DBat2L::MoveTo(B2Mem1CachedEx.lo);
      DBat2U::MoveTo(B2Mem1CachedEx.up);
      ISync();
      Sync();
    }
  }

  if (mem2Size >= BatBits::MIN_SIZE) {
    // Create up to 3 blocks for MEM2
    mem2Size = adjust_block_size(mem2Size, 3);
    u32 blockSize = bit_floor(mem2Size);

    // BAT4 - MEM2 Cached
    const BatBits B4Mem2Cached = {
        {
            .BEPI = 0x90000000 >> BatBits::ADDRESS_SHIFT,
            .BL = (blockSize - 1) >> BatBits::ADDRESS_SHIFT,
            .VS = 1,
            .VP = 1,
        },
        {
            .BRPN = 0x10000000 >> BatBits::ADDRESS_SHIFT,
            .WIMG = 0b0000,
            .PP = 2,
        },
    };

    if (allowExecInMem2) {
      IBat4L::MoveTo(B4Mem2Cached.lo);
      IBat4U::MoveTo(B4Mem2Cached.up);
      ISync();
    }
    DBat4L::MoveTo(B4Mem2Cached.lo);
    DBat4U::MoveTo(B4Mem2Cached.up);
    ISync();
    Sync();

    // BAT5 - MEM2 Uncached
    // Always rounded up to power of two.
    const BatBits B5Mem2Uncached = {
        {
            .BEPI = 0xD0000000 >> BatBits::ADDRESS_SHIFT,
            .BL = (bit_ceil(mem2Size) - 1) >> BatBits::ADDRESS_SHIFT,
            .VS = 1,
            .VP = 1,
        },
        {
            .BRPN = 0x10000000 >> BatBits::ADDRESS_SHIFT,
            .WIMG = 0b0101,
            .PP = 2,
        },
    };

    if (allowExecInUncached && allowExecInMem2) {
      IBat5L::MoveTo(B5Mem2Uncached.lo);
      IBat5U::MoveTo(B5Mem2Uncached.up);
      ISync();
    }
    DBat5L::MoveTo(B5Mem2Uncached.lo);
    DBat5U::MoveTo(B5Mem2Uncached.up);
    ISync();
    Sync();

    // Extend MEM2 to another bit of size
    u32 blockAddress = blockSize;
    blockSize = bit_floor(mem2Size & (blockSize - 1));

    if (blockSize >= BatBits::MIN_SIZE) {
      // BAT6 - MEM2 Cached Block 2
      const BatBits B6Mem2Cached = {
          {
              .BEPI = (0x90000000 + blockAddress) >> BatBits::ADDRESS_SHIFT,
              .BL = (blockSize - 1) >> BatBits::ADDRESS_SHIFT,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = (0x10000000 + blockAddress) >> BatBits::ADDRESS_SHIFT,
              .WIMG = 0b0000,
              .PP = 2,
          },
      };

      if (allowExecInMem2) {
        IBat6L::MoveTo(B6Mem2Cached.lo);
        IBat6U::MoveTo(B6Mem2Cached.up);
        ISync();
      }
      DBat6L::MoveTo(B6Mem2Cached.lo);
      DBat6U::MoveTo(B6Mem2Cached.up);
      ISync();
      Sync();

      // And then another bit
      blockAddress = blockAddress | blockSize;
      blockSize = bit_floor(mem2Size & (blockSize - 1));

      if (blockSize >= BatBits::MIN_SIZE) {
        const BatBits B7Mem2Cached = {
            {
                .BEPI = (0x90000000 + blockAddress) >> BatBits::ADDRESS_SHIFT,
                .BL = (blockSize - 1) >> BatBits::ADDRESS_SHIFT,
                .VS = 1,
                .VP = 1,
            },
            {
                .BRPN = (0x10000000 + blockAddress) >> BatBits::ADDRESS_SHIFT,
                .WIMG = 0b0000,
                .PP = 2,
            },
        };

        if (allowExecInMem2) {
          IBat7L::MoveTo(B7Mem2Cached.lo);
          IBat7U::MoveTo(B7Mem2Cached.up);
          ISync();
        }
        DBat7L::MoveTo(B7Mem2Cached.lo);
        DBat7U::MoveTo(B7Mem2Cached.up);
        ISync();
        Sync();
      }
    }
  }
}

} // namespace peli::ppc