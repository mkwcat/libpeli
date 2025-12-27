// peli/ppc/Hid4.hpp - Hardware Implementation-Dependent 4
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "SprInterface.hpp"

namespace peli::ppc {

struct Hid4Bits {
  static constexpr Spr SPR = Spr::HID4;

  /* 0 */ u32 H4A : 1 = 0;
  /* 1-2 */ u32 L2FM : 2 = 0;
  /* 3-4 */ u32 BPD : 2 = 0;
  /* 5 */ u32 RESV26 : 1 = 0;
  /* 6 */ u32 SBE : 1 = 0;
  /* 7 */ u32 ST0 : 1 = 0;
  /* 8 */ u32 LPE : 1 = 0;
  /* 9 */ u32 DBP : 1 = 0;
  /* 10 */ u32 L2MUM : 1 = 0;
  /* 11 */ u32 L2CFI : 1 = 0;
  /* 12-31 */ u32 RESV0 : 20 = 0;
};

using Hid4 = SprInterface<Hid4Bits>;

} // namespace peli::ppc