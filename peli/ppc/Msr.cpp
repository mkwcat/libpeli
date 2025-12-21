// peli/ppc/Msr.cpp - Machine State Register
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Msr.hpp"
#include "../common/Asm.h"
#include "../common/AsmRegisters.h"

namespace peli::ppc {

PELI_ASM_METHOD( // clang-format off
  [[gnu::weak]]
  u32 Msr::EnterRealMode() noexcept,
  (),

  mfmsr   r3;

  mflr    r4;
  rlwinm  r4, r4, 0, ~0xC0000000;
  mtsrr0  r4;

  li      r0, 0;
  mtsrr1  r0;

  rlwinm  r1, r1, 0, ~0xC0000000;
  rlwinm  r2, r2, 0, ~0xC0000000;
  rlwinm  r13, r13, 0, ~0xC0000000;

  rfi;
                 // clang-format on
);

PELI_ASM_METHOD( // clang-format off
  [[gnu::weak]]
  void Msr::ExitRealMode([[maybe_unused]] u32 prevMsr) noexcept,
  (),

  mflr    r4;
  oris    r4, r4, 0x80000000@h;
  mtsrr0  r4;

  mtsrr1  r3;

  oris    r1, r1, 0x80000000@h;
  oris    r2, r2, 0x80000000@h;
  oris    r13, r13, 0x80000000@h;

  rfi;
                 // clang-format on
);

} // namespace peli::ppc