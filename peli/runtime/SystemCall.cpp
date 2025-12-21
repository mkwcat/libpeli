// peli/runtime/SystemCall.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "SystemCall.hpp"
#include "../common/Asm.h"
#include "../ppc/Spr.hpp"

#define BIG_ORDER_BIT(n) (1 << (31 - (n)))

namespace peli::runtime::SystemCall::detail {

PELI_ASM_FUNCTION( // clang-format off
  void SystemCallHandler() noexcept,

  // Branch if bit 28 is true
  bc      0b01100, 28, .SC_DisableInterrupts;
  // Branch if bit 29 is true
  bc      0b01100, 29, .SC_DcLockAndFlush;

.SC_Sync:;
  mfspr   %r0, PELI_SPR_HID0;
  ori     %r3, %r0, BIG_ORDER_BIT(28); // Set HID0.ABE
  mtspr   PELI_SPR_HID0, %r3; // Write back to HID0
  isync;
  sync;
  mtspr   PELI_SPR_HID0, %r0; // Restore HID0
  rfi;

.SC_DisableInterrupts:;
  // r0 = 0 disable, 1 = enable
  mfspr   %r0, PELI_SPR_SRR1; // Stored MSR
  rlwinm  %r0, %r0, 0, ~BIG_ORDER_BIT(16); // Clear MSR.EE
  mtspr   PELI_SPR_SRR1, %r0;
  rfi;

.SC_DcLockAndFlush:;
  mfspr   %r0, PELI_SPR_HID2;
  ori     %r0, %r0, BIG_ORDER_BIT(19); // Set HID0.DLOCK
  mtspr   PELI_SPR_HID2, %r0;

  // Flush the entire cache by filling up all 8 ways of 128 sets
  li      %r3, 128 * 8 * 32 - 31;
  dcbst   0, %r3;
  dcbz    0, %r3;
  subi    %r3, %r3, 32;
  cmpwi   %cr7, %r3, 0;
  bgt+    %cr7, -0xC;

  // Remove zeroed data cache blocks through flash invalidate, and address broadcast
  mfspr   %r0, PELI_SPR_HID0;
  ori     %r3, %r0, BIG_ORDER_BIT(28) | BIG_ORDER_BIT(18); // Set HID0.ABE and HID0.DCFI
  mtspr   PELI_SPR_HID0, %r3; // Write back to HID0
  isync;
  sync;
  mtspr   PELI_SPR_HID0, %r0; // Restore HID0
  rfi;

  .long   0; // Terminator
                   // clang-format on
);

} // namespace peli::runtime::SystemCall::detail