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
  static constexpr int AddressShift = 17;
  static constexpr u32 MinSize = 0x20000;

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

struct BatConfig {
  BatBits m_ibat[8];
  BatBits m_dbat[8];

  constexpr BatConfig() = default;

  constexpr BatConfig(u32 mem1_size, u32 mem2_size,
                      [[maybe_unused]] bool locked_cache,
                      bool allow_exec_in_mem2,
                      bool allow_exec_in_uncached) noexcept {
    const auto bit_floor = [](u32 value) constexpr -> u32 {
      if (value == 0) {
        return 0;
      }

      return 1u << (31 - __builtin_clz(value));
    };
    const auto bit_ceil = [](u32 value) constexpr -> u32 {
      if (value == 0 || value == 1) {
        return 1;
      }

      return 1u << (32 - __builtin_clz(value - 1));
    };

    // Adjust to the minimum size we can expect, in case the caller expects more
    // than the allocated number of blocks.
    const auto adjust_block_size = [bit_floor](u32 size,
                                               int count) constexpr -> u32 {
      u32 corrected = 0, last_bit = 0, floored;

      for (int i = 0;
           (floored = bit_floor(size & (last_bit - 1))) >= BatBits::MinSize &&
           i < count;
           i++) {
        last_bit = floored;
        corrected |= floored;
      }

      if (floored != 0) {
        corrected += last_bit;
      }

      return corrected;
    };

    if (mem1_size >= BatBits::MinSize) {
      // Create up to 2 blocks for MEM1
      mem1_size = adjust_block_size(mem1_size, 2);
      u32 block_size = bit_floor(mem1_size);

      // BAT0 - MEM1 Cached
      m_dbat[0] = m_ibat[0] = {
          {
              .BEPI = 0x80000000 >> BatBits::AddressShift,
              .BL = (block_size - 1) >> BatBits::AddressShift,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = 0x00000000 >> BatBits::AddressShift,
              .WIMG = 0b0000,
              .PP = 2,
          },
      };

      // BAT1 - MEM1 Uncached and Hardware Interfaces
      // This is always 256 MB
      m_dbat[1] = {
          {
              .BEPI = 0xC0000000 >> BatBits::AddressShift,
              .BL = 0x0FFFFFFF >> BatBits::AddressShift,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = 0x00000000 >> BatBits::AddressShift,
              .WIMG = 0b0101,
              .PP = 2,
          },
      };

      if (allow_exec_in_uncached) {
        m_ibat[1] = m_dbat[1];
      }

      // Extend MEM1 to an extra bit of size
      u32 block_address = block_size;
      block_size = bit_floor(mem1_size & (block_size - 1));

      if (block_size >= BatBits::MinSize) {
        // BAT2 - Extra MEM1 Cached
        m_dbat[2] = m_ibat[2] = {
            {
                .BEPI = (0x80000000 + block_address) >> BatBits::AddressShift,
                .BL = (block_size - 1) >> BatBits::AddressShift,
                .VS = 1,
                .VP = 1,
            },
            {
                .BRPN = block_address >> BatBits::AddressShift,
                .WIMG = 0b0000,
                .PP = 2,
            },
        };
      }
    }

    if (mem2_size >= BatBits::MinSize) {
      // Create up to 3 blocks for MEM2
      mem2_size = adjust_block_size(mem2_size, 3);
      u32 block_size = bit_floor(mem2_size);

      // BAT4 - MEM2 Cached
      m_dbat[4] = {
          {
              .BEPI = 0x90000000 >> BatBits::AddressShift,
              .BL = (block_size - 1) >> BatBits::AddressShift,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = 0x10000000 >> BatBits::AddressShift,
              .WIMG = 0b0000,
              .PP = 2,
          },
      };

      if (allow_exec_in_mem2) {
        m_ibat[4] = m_dbat[4];
      }

      // BAT5 - MEM2 Uncached
      // Always rounded up to power of two.
      m_dbat[5] = {
          {
              .BEPI = 0xD0000000 >> BatBits::AddressShift,
              .BL = (bit_ceil(mem2_size) - 1) >> BatBits::AddressShift,
              .VS = 1,
              .VP = 1,
          },
          {
              .BRPN = 0x10000000 >> BatBits::AddressShift,
              .WIMG = 0b0101,
              .PP = 2,
          },
      };

      if (allow_exec_in_uncached && allow_exec_in_mem2) {
        m_ibat[5] = m_dbat[5];
      }

      // Extend MEM2 to another bit of size
      u32 block_address = block_size;
      block_size = bit_floor(mem2_size & (block_size - 1));

      if (block_size >= BatBits::MinSize) {
        // BAT6 - MEM2 Cached Block 2
        m_dbat[6] = {
            {
                .BEPI = (0x90000000 + block_address) >> BatBits::AddressShift,
                .BL = (block_size - 1) >> BatBits::AddressShift,
                .VS = 1,
                .VP = 1,
            },
            {
                .BRPN = (0x10000000 + block_address) >> BatBits::AddressShift,
                .WIMG = 0b0000,
                .PP = 2,
            },
        };

        if (allow_exec_in_mem2) {
          m_ibat[6] = m_dbat[6];
        }

        // And then another bit
        block_address = block_address | block_size;
        block_size = bit_floor(mem2_size & (block_size - 1));

        if (block_size >= BatBits::MinSize) {
          m_dbat[7] = {
              {
                  .BEPI = (0x90000000 + block_address) >> BatBits::AddressShift,
                  .BL = (block_size - 1) >> BatBits::AddressShift,
                  .VS = 1,
                  .VP = 1,
              },
              {
                  .BRPN = (0x10000000 + block_address) >> BatBits::AddressShift,
                  .WIMG = 0b0000,
                  .PP = 2,
              },
          };

          if (allow_exec_in_mem2) {
            m_ibat[7] = m_dbat[7];
          }
        }
      }
    }
  }

  constexpr u32 Upper(bool data, int index) const {
    return __builtin_bit_cast(u32, data ? m_dbat[index].up : m_ibat[index].up);
  }
  constexpr u32 Lower(bool data, int index) const {
    return __builtin_bit_cast(u32, data ? m_dbat[index].lo : m_ibat[index].lo);
  }
};

template <BatConfig C>
[[gnu::optimize("-Os")]] [[gnu::optimize("-fno-schedule-insns")]]
inline void BatConfigure() {
  DBat0L::MoveTo(C.Lower(true, 0));
  DBat0U::MoveTo(C.Upper(true, 0));
  ISync();
  IBat0L::MoveTo(C.Lower(false, 0));
  IBat0U::MoveTo(C.Upper(false, 0));
  ISync();
  DBat1L::MoveTo(C.Lower(true, 1));
  DBat1U::MoveTo(C.Upper(true, 1));
  ISync();
  IBat1L::MoveTo(C.Lower(false, 1));
  IBat1U::MoveTo(C.Upper(false, 1));
  ISync();
  DBat2L::MoveTo(C.Lower(true, 2));
  DBat2U::MoveTo(C.Upper(true, 2));
  ISync();
  IBat2L::MoveTo(C.Lower(false, 2));
  IBat2U::MoveTo(C.Upper(false, 2));
  ISync();
  DBat3L::MoveTo(C.Lower(true, 3));
  DBat3U::MoveTo(C.Upper(true, 3));
  ISync();
  IBat3L::MoveTo(C.Lower(false, 3));
  IBat3U::MoveTo(C.Upper(false, 3));
  ISync();
  DBat4L::MoveTo(C.Lower(true, 4));
  DBat4U::MoveTo(C.Upper(true, 4));
  ISync();
  IBat4L::MoveTo(C.Lower(false, 4));
  IBat4U::MoveTo(C.Upper(false, 4));
  ISync();
  DBat5L::MoveTo(C.Lower(true, 5));
  DBat5U::MoveTo(C.Upper(true, 5));
  ISync();
  IBat5L::MoveTo(C.Lower(false, 5));
  IBat5U::MoveTo(C.Upper(false, 5));
  ISync();
  DBat6L::MoveTo(C.Lower(true, 6));
  DBat6U::MoveTo(C.Upper(true, 6));
  ISync();
  IBat6L::MoveTo(C.Lower(false, 6));
  IBat6U::MoveTo(C.Upper(false, 6));
  ISync();
  DBat7L::MoveTo(C.Lower(true, 7));
  DBat7U::MoveTo(C.Upper(true, 7));
  ISync();
  IBat7L::MoveTo(C.Lower(false, 7));
  IBat7U::MoveTo(C.Upper(false, 7));
  ISync();
}

} // namespace peli::ppc