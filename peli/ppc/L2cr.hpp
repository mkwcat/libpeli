// peli/ppc/L2cr.hpp - L2 Control Register
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "SprInterface.hpp"

namespace peli::ppc {

struct L2crBits {
  static constexpr Spr SPR = Spr::L2CR;

  /* 0 */ u32 L2E : 1 = 0;
  /* 1 */ u32 L2CE : 1 = 0;
  /* 2-8 */ u32 L2P : 7 = 0;
  /* 9 */ u32 L2DO : 1 = 0;
  /* 10 */ u32 L2I : 1 = 0;
  /* 11 */ u32 RESV11 : 1 = 0;
  /* 12 */ u32 L2WT : 1 = 0;
  /* 13 */ u32 L2TS : 1 = 0;
  /* 14-30 */ u32 RESV14 : 1 = 0;
  /* 31 */ u32 L2IP : 1 = 0;
};

using L2cr = SprInterface<L2crBits>;

} // namespace peli::ppc