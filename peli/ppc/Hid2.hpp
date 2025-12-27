// peli/ppc/Hid2.hpp - Hardware Implementation-Dependent 2
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "SprInterface.hpp"

namespace peli::ppc {

struct Hid2Bits {
  static constexpr Spr SPR = Spr::HID2;

  /* 0 */ u32 LSQE : 1 = 0;
  /* 1 */ u32 WPE : 1 = 0;
  /* 2 */ u32 PSE : 1 = 0;
  /* 3 */ u32 LCE : 1 = 0;
  /* 4-7 */ u32 DMAQL : 4 = 0;
  /* 8 */ u32 DCHERR : 1 = 0;
  /* 9 */ u32 DNCERR : 1 = 0;
  /* 10 */ u32 DCMERR : 1 = 0;
  /* 11 */ u32 DQOERR : 1 = 0;
  /* 12 */ u32 DCHEE : 1 = 0;
  /* 13 */ u32 DNCEE : 1 = 0;
  /* 14 */ u32 DCMEE : 1 = 0;
  /* 15 */ u32 DQOEE : 1 = 0;
  /* 16-31 */ u32 RESV16 : 16 = 0;
};

using Hid2 = SprInterface<Hid2Bits>;

} // namespace peli::ppc