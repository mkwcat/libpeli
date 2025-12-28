// peli/ppc/Context.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Context.hpp"
#include "../cmn/Asm.h"
#include "../cmn/AsmRegisters.h"
#include "../host/Config.h"
#include "Spr.hpp"

namespace peli::ppc {

#define RM_CURRENT_CONTEXT 0x00C0
#define EM_CURRENT_CONTEXT 0x00D4

#define CODE_IF_ENABLE_FLOAT(...)
#define CODE_IF_ENABLE_PAIRED_SINGLE(...)
#define CODE_IF_NOT_ENABLE_PAIRED_SINGLE(...) __VA_ARGS__

#if defined(PELI_ENABLE_FLOAT)
#undef CODE_IF_ENABLE_FLOAT
#define CODE_IF_ENABLE_FLOAT(...) __VA_ARGS__
#endif

#if defined(PELI_ENABLE_PAIRED_SINGLE)
#undef CODE_IF_ENABLE_PAIRED_SINGLE
#define CODE_IF_ENABLE_PAIRED_SINGLE(...) __VA_ARGS__
#undef CODE_IF_NOT_ENABLE_PAIRED_SINGLE
#define CODE_IF_NOT_ENABLE_PAIRED_SINGLE(...)
#endif

PELI_ASM_METHOD( // clang-format off
  [[noreturn]]
  void Context::Restore() noexcept,
  (),

.L_ContextRestore:;
  lwz     r0, 0x080(r3); // OSContext.cr
  lwz     r4, 0x084(r3); // OSContext.lr
  mtcr    r0;
  lwz     r5, 0x088(r3); // OSContext.ctr
  mtlr    r4;
  lwz     r0, 0x090(r3); // OSContext.xer
  mtctr   r5;
  lwz     r4, 0x198(r3); // OSContext.srr0
  mtxer   r0;
  lwz     r5, 0x19C(r3); // OSContext.srr1
  mtsrr0  r4;
  rlwinm  r5, r5, 0, ~0x2000; // Clear MSR.FP
  lwz     r0, 0x1A8(r3); // OSContext.gqrs[1]
  mtsrr1  r5;
  lwz     r4, 0x1AC(r3); // OSContext.gqrs[2]
  mtspr   PELI_SPR_GQR1, r0;
  lwz     r5, 0x1B0(r3); // OSContext.gqrs[3]
  mtspr   PELI_SPR_GQR2, r4;
  lwz     r0, 0x1B4(r3); // OSContext.gqrs[4]
  mtspr   PELI_SPR_GQR3, r5;
  lwz     r4, 0x1B8(r3); // OSContext.gqrs[5]
  mtspr   PELI_SPR_GQR4, r0;
  lwz     r5, 0x1BC(r3); // OSContext.gqrs[6]
  mtspr   PELI_SPR_GQR5, r4;
  lwz     r0, 0x1C0(r3); // OSContext.gqrs[7]
  mtspr   PELI_SPR_GQR6, r5;
  lwz     r1, 0x004(r3); // OSContext.gprs[1]
CODE_IF_ENABLE_PAIRED_SINGLE(
  lwz     r4, 0x1C4(r3); // OSContext.hid2
  mfhid2  r5;
  lwz     r2, 0x008(r3); // OSContext.gprs[2]
  rlwimi  r5, r4, 0, 0x80000000; // Insert HID2.LSQE
  rlwimi  r5, r4, 0, 0x20000000; // Insert HID2.PSE
  mtspr   PELI_SPR_GQR7, r0;
  mthid2  r5;
) // CODE_IF_ENABLE_PAIRED_SINGLE
CODE_IF_NOT_ENABLE_PAIRED_SINGLE(
  lwz     r2, 0x008(r3); // OSContext.gprs[2]
  mtspr   PELI_SPR_GQR7, r0;
) // CODE_IF_NOT_ENABLE_PAIRED_SINGLE
  lwz     r0, 0x000(r3); // OSContext.gprs[0]
  lmw     r4, 0x010(r3); // OSContext.gprs[4+]
  lwz     r3, 0x00C(r3); // OSContext.gprs[3]
  rfi;
                 // clang-format on
);

PELI_ASM_METHOD( // clang-format off
  void Context::FastSwitch() noexcept,
  (),

  lis     r5, 0x8000;
  lwz     r4, EM_CURRENT_CONTEXT(r5);
  cmpwi   r4, 0;
  rlwinm  r6, r3, 0, ~0xC0000000;
  stw     r6, RM_CURRENT_CONTEXT(r5); // Update real mode context
  stw     r3, EM_CURRENT_CONTEXT(r5); // Update effective mode context
  beq-    .L%=Restore;

  // Only bother saving r1, r2, r13+ as this function follows the ABI
  // This is, by the way, critical for the external interrupt handler to work
  stw     r1, 0x004(r4); // OSContext.gprs[1]
  stw     r2, 0x008(r4); // OSContext.gprs[2]
  mfcr    r0;
  stmw    r13, 0x034(r4); // OSContext.gprs[13+]
  mflr    r5;
  stw     r0, 0x080(r4); // OSContext.cr
  // Skip CTR
  mfxer   r6;
  stw     r5, 0x084(r4); // OSContext.lr
  mfmsr   r0;
  stw     r5, 0x198(r4); // OSContext.srr0
CODE_IF_ENABLE_FLOAT(
CODE_IF_ENABLE_PAIRED_SINGLE(
  mfhid2  r8; // OSContext.hid2
  andis.  r7, r8, 0xA000; // Get HID2.PSE and HID2.LSQE
  stw     r7, 0x1C4(r4); // OSContext.hid2
) // CODE_IF_ENABLE_PAIRED_SINGLE
  rlwinm. r7, r0, 0, 0x2000; // CR0.EQ = MSR.FP
) // CODE_IF_ENABLE_FLOAT
  stw     r6, 0x090(r4); // OSContext.xer
  mfspr   r5, PELI_SPR_GQR0;
  stw     r0, 0x19C(r4); // OSContext.srr1
  mfspr   r6, PELI_SPR_GQR1;
  stw     r5, 0x1A4(r4); // OSContext.gqrs[0]
  mfspr   r0, PELI_SPR_GQR2;
  stw     r6, 0x1A8(r4); // OSContext.gqrs[1]
  mfspr   r5, PELI_SPR_GQR3;
  stw     r0, 0x1AC(r4); // OSContext.gqrs[2]
  mfspr   r6, PELI_SPR_GQR4;
  stw     r5, 0x1B0(r4); // OSContext.gqrs[3]
  mfspr   r0, PELI_SPR_GQR5;
  stw     r6, 0x1B4(r4); // OSContext.gqrs[4]
  mfspr   r5, PELI_SPR_GQR6;
  stw     r0, 0x1B8(r4); // OSContext.gqrs[5]
  mfspr   r6, PELI_SPR_GQR7;
  stw     r5, 0x1BC(r4); // OSContext.gqrs[6]
  stw     r6, 0x1C0(r4); // OSContext.gqrs[7]

CODE_IF_ENABLE_FLOAT(
  beq+    .L%=Restore;

  // Save floating point registers
  stfd    f0, 0x090(r4); // OSContext.fprs[0]
  mffs    f0;
  stfd    f1, 0x098(r4); // OSContext.fprs[1]
  stfd    f0, 0x190(r4); // OSContext.fpscr
CODE_IF_ENABLE_PAIRED_SINGLE(
  xoris   r0, r8, 0xA000;
) // CODE_IF_ENABLE_PAIRED_SINGLE
  stfd    f2, 0x0A0(r4); // OSContext.fprs[2]
  stfd    f3, 0x0A8(r4); // OSContext.fprs[3]
  stfd    f4, 0x0B0(r4); // OSContext.fprs[4]
  stfd    f5, 0x0B8(r4); // OSContext.fprs[5]
  stfd    f6, 0x0C0(r4); // OSContext.fprs[6]
  stfd    f7, 0x0C8(r4); // OSContext.fprs[7]
  stfd    f8, 0x0D0(r4); // OSContext.fprs[8]
CODE_IF_ENABLE_PAIRED_SINGLE(
  cmplwi  r0, 0;
) // CODE_IF_ENABLE_PAIRED_SINGLE
  stfd    f9, 0x0D8(r4); // OSContext.fprs[9]
  stfd    f10, 0x0E0(r4); // OSContext.fprs[10]
  stfd    f11, 0x0E8(r4); // OSContext.fprs[11]
  stfd    f12, 0x0F0(r4); // OSContext.fprs[12]
  stfd    f13, 0x0F8(r4); // OSContext.fprs[13]
  stfd    f14, 0x100(r4); // OSContext.fprs[14]
  stfd    f15, 0x108(r4); // OSContext.fprs[15]
  stfd    f16, 0x110(r4); // OSContext.fprs[16]
  stfd    f17, 0x118(r4); // OSContext.fprs[17]
  stfd    f18, 0x120(r4); // OSContext.fprs[18]
  stfd    f19, 0x128(r4); // OSContext.fprs[19]
  stfd    f20, 0x130(r4); // OSContext.fprs[20]
  stfd    f21, 0x138(r4); // OSContext.fprs[21]
  stfd    f22, 0x140(r4); // OSContext.fprs[22]
  stfd    f23, 0x148(r4); // OSContext.fprs[23]
  stfd    f24, 0x150(r4); // OSContext.fprs[24]
  stfd    f25, 0x158(r4); // OSContext.fprs[25]
  stfd    f26, 0x160(r4); // OSContext.fprs[26]
  stfd    f27, 0x168(r4); // OSContext.fprs[27]
  stfd    f28, 0x170(r4); // OSContext.fprs[28]
  stfd    f29, 0x178(r4); // OSContext.fprs[29]
  stfd    f30, 0x180(r4); // OSContext.fprs[30]
  stfd    f31, 0x188(r4); // OSContext.fprs[31]

CODE_IF_ENABLE_PAIRED_SINGLE(
  bne+    .L%=Restore;

  // Save paired single registers
  psq_st  f0, 0x1C8(r4), 0, 0; // OSContext.psfs[0]
  psq_st  f1, 0x1D0(r4), 0, 0; // OSContext.psfs[1]
  psq_st  f2, 0x1D8(r4), 0, 0; // OSContext.psfs[2]
  psq_st  f3, 0x1E0(r4), 0, 0; // OSContext.psfs[3]
  psq_st  f4, 0x1E8(r4), 0, 0; // OSContext.psfs[4]
  psq_st  f5, 0x1F0(r4), 0, 0; // OSContext.psfs[5]
  psq_st  f6, 0x1F8(r4), 0, 0; // OSContext.psfs[6]
  psq_st  f7, 0x200(r4), 0, 0; // OSContext.psfs[7]
  psq_st  f8, 0x208(r4), 0, 0; // OSContext.psfs[8]
  psq_st  f9, 0x210(r4), 0, 0; // OSContext.psfs[9]
  psq_st  f10, 0x218(r4), 0, 0; // OSContext.psfs[10]
  psq_st  f11, 0x220(r4), 0, 0; // OSContext.psfs[11]
  psq_st  f12, 0x228(r4), 0, 0; // OSContext.psfs[12]
  psq_st  f13, 0x230(r4), 0, 0; // OSContext.psfs[13]
  psq_st  f14, 0x238(r4), 0, 0; // OSContext.psfs[14]
  psq_st  f15, 0x240(r4), 0, 0; // OSContext.psfs[15]
  psq_st  f16, 0x248(r4), 0, 0; // OSContext.psfs[16]
  psq_st  f17, 0x250(r4), 0, 0; // OSContext.psfs[17]
  psq_st  f18, 0x258(r4), 0, 0; // OSContext.psfs[18]
  psq_st  f19, 0x260(r4), 0, 0; // OSContext.psfs[19]
  psq_st  f20, 0x268(r4), 0, 0; // OSContext.psfs[20]
  psq_st  f21, 0x270(r4), 0, 0; // OSContext.psfs[21]
  psq_st  f22, 0x278(r4), 0, 0; // OSContext.psfs[22]
  psq_st  f23, 0x280(r4), 0, 0; // OSContext.psfs[23]
  psq_st  f24, 0x288(r4), 0, 0; // OSContext.psfs[24]
  psq_st  f25, 0x290(r4), 0, 0; // OSContext.psfs[25]
  psq_st  f26, 0x298(r4), 0, 0; // OSContext.psfs[26]
  psq_st  f27, 0x2A0(r4), 0, 0; // OSContext.psfs[27]
  psq_st  f28, 0x2A8(r4), 0, 0; // OSContext.psfs[28]
  psq_st  f29, 0x2B0(r4), 0, 0; // OSContext.psfs[29]
  psq_st  f30, 0x2B8(r4), 0, 0; // OSContext.psfs[30]
  psq_st  f31, 0x2C0(r4), 0, 0; // OSContext.psfs[31]
) // CODE_IF_ENABLE_PAIRED_SINGLE

) // CODE_IF_ENABLE_FLOAT

.L%=Restore:;
  b       .L_ContextRestore;
                 // clang-format on
);

PELI_ASM_METHOD( // clang-format off
  bool Context::Save() noexcept,
  (),

  stw     r0, 0x000(r3); // OSContext.gprs[0]
  li      r5, 0;
  stw     r1, 0x004(r3); // OSContext.gprs[1]
  mfcr    r0;
  stw     r2, 0x008(r3); // OSContext.gprs[2]
  stw     r5, 0x00C(r3); // OSContext.gprs[3]
  stmw    r4, 0x010(r3); // OSContext.gprs[4+]

  stw     r0, 0x080(r3); // OSContext.cr
  mflr    r4;
  stw     r4, 0x084(r3); // OSContext.lr
  mfctr   r5;
  stw     r4, 0x198(r3); // OSContext.srr0
  mfxer   r0;
  stw     r5, 0x088(r3); // OSContext.ctr
  stw     r0, 0x090(r3); // OSContext.xer
  mfmsr   r0;
  mfspr   r4, PELI_SPR_GQR0;
  stw     r0, 0x19C(r3); // OSContext.srr1
  mfspr   r5, PELI_SPR_GQR1;
  stw     r4, 0x1A4(r3); // OSContext.gqrs[0]
  mfspr   r0, PELI_SPR_GQR2;
  stw     r5, 0x1A8(r3); // OSContext.gqrs[1]
  mfspr   r4, PELI_SPR_GQR3;
  stw     r0, 0x1AC(r3); // OSContext.gqrs[2]
  mfspr   r5, PELI_SPR_GQR4;
  stw     r4, 0x1B0(r3); // OSContext.gqrs[3]
  mfspr   r0, PELI_SPR_GQR5;
  stw     r5, 0x1B4(r3); // OSContext.gqrs[4]
  mfspr   r4, PELI_SPR_GQR6;
  stw     r0, 0x1B8(r3); // OSContext.gqrs[5]
  mfspr   r5, PELI_SPR_GQR7;
  stw     r4, 0x1BC(r3); // OSContext.gqrs[6]
  li      r3, 1;
  stw     r5, 0x1C0(r3); // OSContext.gqrs[7]
  blr;
                 // clang-format on
);

} // namespace peli::ppc