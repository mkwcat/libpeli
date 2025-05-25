// wsh/ppc/Spr.hpp - Special Purpose Registers
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"

namespace wsh::ppc {

enum class Spr {
  XER = 1,
  LR = 8,
  CTR = 9,
  DSISR = 18,
  DAR = 19,
  DEC = 22,
  SDR = 25,
  SRR0 = 26,
  SRR1 = 27,
  PVR = 287,
  GQR0 = 912,
  GQR1 = 913,
  GQR2 = 914,
  GQR3 = 915,
  GQR4 = 916,
  GQR5 = 917,
  GQR6 = 918,
  GQR7 = 919,
  HID2 = 920,
  HID5 = 944,
  BCR = 949,
  HID0 = 1008,
  HID4 = 1011,
  L2CR = 1017,
  IBAT0U = 528,
  IBAT0L = 529,
  IBAT1U = 530,
  IBAT1L = 531,
  IBAT2U = 532,
  IBAT2L = 533,
  IBAT3U = 534,
  IBAT3L = 535,
  DBAT0U = 536,
  DBAT0L = 537,
  DBAT1U = 538,
  DBAT1L = 539,
  DBAT2U = 540,
  DBAT2L = 541,
  DBAT3U = 542,
  DBAT3L = 543,
  IBAT4U = 560,
  IBAT4L = 561,
  IBAT5U = 562,
  IBAT5L = 563,
  IBAT6U = 564,
  IBAT6L = 565,
  IBAT7U = 566,
  IBAT7L = 567,
  DBAT4U = 568,
  DBAT4L = 569,
  DBAT5U = 570,
  DBAT5L = 571,
  DBAT6U = 572,
  DBAT6L = 573,
  DBAT7U = 574,
  DBAT7L = 575,

  // Not actually an SPR, but used internally by this library
  MSR = 0x8000,
};

template <Spr Reg> static inline u32 SetSpr(u32 value) noexcept {
  if constexpr (Reg == Spr::MSR) {
    __asm__ __volatile__("mtmsr %0;" : : "r"(value));
    return value;
  } else {
    __asm__ __volatile__("mtspr %0, %1;"
                         :
                         : "i"(static_cast<u32>(Reg)), "r"(value));
    return value;
  }
}

template <Spr Reg> static inline u32 GetSpr() noexcept {
  u32 value;
  if (Reg == Spr::MSR) {
    __asm__ __volatile__("mfmsr %0;" : "=r"(value));
    return value;
  } else {
    __asm__ __volatile__("mfspr %0, %1;"
                         : "=r"(value)
                         : "i"(static_cast<u32>(Reg)));
  }
  return value;
}

template <Spr Reg> static inline u32 ClearSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() & ~value);
}

template <Spr Reg> static inline u32 SetSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() | value);
}

} // namespace wsh::ppc