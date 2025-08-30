// peli/ppc/Hid0.hpp - Hardware Implementation-Dependent 0
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "Spr.hpp"
#include "SprRwCtl.hpp"
#include "detail/SprInterface.hpp"

namespace peli::ppc {

struct Hid0Bits {
  static constexpr Spr SPR = Spr::HID0;

  /* 0 */ u32 EMCP : 1 = 0;
  /* 1 */ u32 DBP : 1 = 0;
  /* 2 */ u32 EBA : 1 = 0;
  /* 3 */ u32 EBD : 1 = 0;
  /* 4 */ u32 BCLK : 1 = 0;
  /* 5 */ u32 RESV5 : 1 = 0;
  /* 6 */ u32 ECLK : 1 = 0;
  /* 7 */ u32 PAR : 1 = 0;

  /**
   * Doze Mode Available.
   */
  /* 8 */ u32 DOZE : 1 = 0;

  /* 9 */ u32 NAP : 1 = 0;
  /* 10 */ u32 SLEEP : 1 = 0;
  /* 11 */ u32 DPM : 1 = 0;
  /* 12-14 */ u32 RESV17 : 3 = 0;

  /**
   * Not Hard Reset. This bit is only an indicator for software.
   */
  /* 15 */ u32 NHR : 1 = 0;

  /**
   * Instruction Cache Enable.
   */
  /* 16 */ u32 ICE : 1 = 0;

  /**
   * Data Cache Enable.
   */
  /* 17 */ u32 DCE : 1 = 0;

  /**
   * Instruction Cache Lock.
   */
  /* 18 */ u32 ILOCK : 1 = 0;

  /**
   * Data Cache Lock.
   */
  /* 19 */ u32 DLOCK : 1 = 0;

  /**
   * Instruction Cache Flash Invalidate.
   */
  /* 20 */ u32 ICFI : 1 = 0;

  /**
   * Data Cache Flash Invalidate.
   */
  /* 21 */ u32 DCFI : 1 = 0;

  /* 22 */ u32 SPD : 1 = 0;
  /* 23 */ u32 IFEM : 1 = 0;

  /**
   * Speculation Guard Enable.
   */
  /* 24 */ u32 SGE : 1 = 0;

  /**
   * Data Cache Flush Assist.
   */
  /* 25 */ u32 DCFA : 1 = 0;

  /**
   * Branch Target Instruction Cache.
   */
  /* 26 */ u32 BTIC : 1 = 0;
  /* 27 */ u32 RESV27 : 1 = 0;

  /**
   * Address Broadcast Enable.
   */
  /* 28 */ u32 ABE : 1 = 0;

  /**
   * Branch History Table.
   */
  /* 29 */ u32 BHT : 1 = 0;

  /* 30 */ u32 RESV30 : 1 = 0;
  /* 31 */ u32 NOOPTI : 1 = 0;
};

using Hid0 = detail::SprInterface<Hid0Bits>;

static inline void EnableSpeculation(bool enable) noexcept {
  SprRwCtl<Hid0>()->SGE = enable;
}

} // namespace peli::ppc