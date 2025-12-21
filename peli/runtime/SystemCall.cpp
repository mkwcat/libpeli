// peli/runtime/SystemCall.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "SystemCall.hpp"
#include "../common/Asm.h"
#include "../common/AsmRegisters.h"
#include "../ppc/Spr.hpp"

#define MSR_EE (1 << 31 >> 16)
#define HID0_ICFI (1 << 31 >> 20)
#define HID0_DCFA (1 << 31 >> 25)
#define HID0_ABE (1 << 31 >> 28)

namespace peli::runtime::SystemCall::detail {

PELI_ASM_METHOD( // clang-format off
  void SystemCallHandler() noexcept,
  (),

  // Branch if bit 28 is true
  bc      0b01100, 28, .L%=DisableInterrupts;
  // Branch if bit 29 is true
  bc      0b01100, 29, .L%=DcFlush;

.L%=Sync:;
  mfspr   r12, PELI_SPR_HID0;
  ori     r11, r12, HID0_ABE;
.L%=SetHID0AndReturn:;
  mtspr   PELI_SPR_HID0, r11;
  isync;
  sync;
  mtspr   PELI_SPR_HID0, r12; // Restore HID0
  rfi;

.L%=DisableInterrupts:;
  mfspr   r11, PELI_SPR_SRR1; // Stored MSR
  rlwinm  r11, r11, 0, ~MSR_EE;
  mtspr   PELI_SPR_SRR1, r11;
  rfi;

.L%=DcFlush:;
  // Set data cache flush assist
  mfspr   r10, PELI_SPR_HID0;
  // r11 = |, r12 = &
  and     r12, r10, r12;
  or      r12, r12, r11;

  ori     r10, r10, HID0_DCFA;
  mtspr   PELI_SPR_HID0, r10;
  isync;

  // Flush the entire L1 data cache by filling up all 8 ways of 128 sets
  li      r11, 128 * 8;
  mtctr   r11;
  lis     r11, 0x8000;
  subi    r11, r11, 32;

  lwzu    r10, 32(r11);
  dcbf    0, r11;
  bdnz+   (-0x8);

  // Enable icache flash invalidate and sync with the 60x bus
  ori     r11, r12, HID0_ICFI | HID0_ABE;
  b       .L%=SetHID0AndReturn;

  .long   0; // Terminator
                 // clang-format on
);

} // namespace peli::runtime::SystemCall::detail