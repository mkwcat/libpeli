// wsh/ppc/Context.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Context.hpp"
#include "../common/Asm.h"

namespace wsh::ppc {

#define GQR0 912
#define GQR1 913
#define GQR2 914
#define GQR3 915
#define GQR4 916
#define GQR5 917
#define GQR6 918
#define GQR7 919

#define RM_CURRENT_CONTEXT 0x00C0
#define EM_CURRENT_CONTEXT 0x00D4

WSH_ASM_FUNCTION( // clang-format off
    [[noreturn]]
    void Context::Restore() noexcept,

L_ContextRestore:;
    lwz     %r0, 0x080(%r3); // OSContext.cr
    lwz     %r4, 0x084(%r3); // OSContext.lr
    mtcr    %r0;
    lwz     %r5, 0x088(%r3); // OSContext.ctr
    mtlr    %r4;
    lwz     %r0, 0x090(%r3); // OSContext.xer
    mtctr   %r5;
    // lwz     %r4, 0x194(%r3); // OSContext.fpscr
    mtxer   %r0;
    lwz     %r5, 0x198(%r3); // OSContext.srr0
    // mtfsf   255, %r4;
    lwz     %r0, 0x19C(%r3); // OSContext.srr1
    mtsrr0  %r5;
    lwz     %r4, 0x1A4(%r3); // OSContext.gqrs[0]
    mtsrr1  %r0;
    lwz     %r5, 0x1A8(%r3); // OSContext.gqrs[1]
    mtspr   GQR0, %r4;
    lwz     %r0, 0x1AC(%r3); // OSContext.gqrs[2]
    mtspr   GQR1, %r5;
    lwz     %r4, 0x1B0(%r3); // OSContext.gqrs[3]
    mtspr   GQR2, %r0;
    lwz     %r5, 0x1B4(%r3); // OSContext.gqrs[4]
    mtspr   GQR3, %r4;
    lwz     %r0, 0x1B8(%r3); // OSContext.gqrs[5]
    mtspr   GQR4, %r5;
    lwz     %r4, 0x1BC(%r3); // OSContext.gqrs[6]
    mtspr   GQR5, %r0;
    lwz     %r5, 0x1C0(%r3); // OSContext.gqrs[7]
    mtspr   GQR6, %r4;
    lwz     %r0, 0x000(%r3); // OSContext.gprs[0]
    lwz     %r1, 0x004(%r3); // OSContext.gprs[1]
    mtspr   GQR7, %r5;
    lmw     %r4, 0x010(%r3); // OSContext.gprs[4+]
    lwz     %r2, 0x008(%r3); // OSContext.gprs[2]
    lwz     %r3, 0x00C(%r3); // OSContext.gprs[3]
    sync;
    isync;
    rfi;
                  // clang-format on
);

WSH_ASM_FUNCTION( // clang-format off
    bool Context::Save() noexcept,

L_ContextSave:;
    stw     %r0, 0x000(%r3); // OSContext.gprs[0]
    li      %r5, 0;
    stw     %r1, 0x004(%r3); // OSContext.gprs[1]
    mfcr    %r0;
    stw     %r2, 0x008(%r3); // OSContext.gprs[2]
    stw     %r5, 0x00C(%r3); // OSContext.gprs[3]
    stmw    %r4, 0x010(%r3); // OSContext.gprs[4+]

    stw     %r0, 0x080(%r3); // OSContext.cr
    mflr    %r4;
    stw     %r4, 0x084(%r3); // OSContext.lr
    mfctr   %r5;
    stw     %r4, 0x198(%r3); // OSContext.srr0
    mfxer   %r0;
    stw     %r5, 0x088(%r3); // OSContext.ctr
    // mffs    %r4;
    stw     %r0, 0x090(%r3); // OSContext.xer
    mfmsr   %r0;
    // stw     %r4, 0x194(%r3); // OSContext.fpscr
    mfspr   %r4, GQR0;
    stw     %r0, 0x19C(%r3); // OSContext.srr1
    mfspr   %r5, GQR1;
    stw     %r4, 0x1A4(%r3); // OSContext.gqrs[0]
    mfspr   %r0, GQR2;
    stw     %r5, 0x1A8(%r3); // OSContext.gqrs[1]
    mfspr   %r4, GQR3;
    stw     %r0, 0x1AC(%r3); // OSContext.gqrs[2]
    mfspr   %r5, GQR4;
    stw     %r4, 0x1B0(%r3); // OSContext.gqrs[3]
    mfspr   %r0, GQR5;
    stw     %r5, 0x1B4(%r3); // OSContext.gqrs[4]
    mfspr   %r4, GQR6;
    stw     %r0, 0x1B8(%r3); // OSContext.gqrs[5]
    mfspr   %r5, GQR7;
    stw     %r4, 0x1BC(%r3); // OSContext.gqrs[6]
    li      %r3, 1;
    stw     %r5, 0x1C0(%r3); // OSContext.gqrs[7]
    blr;
                  // clang-format on
);

WSH_ASM_FUNCTION( // clang-format off
    void Context::FastSwitch() noexcept,

L_ContextSwitch:;
    lis     %r5, 0x8000;
    rlwinm  %r6, %r3, 0, ~0xC0000000;
    lwz     %r4, EM_CURRENT_CONTEXT(%r5);
    stw     %r6, RM_CURRENT_CONTEXT(%r5); // Update real mode context
    cmpwi   %r4, 0;
    stw     %r3, EM_CURRENT_CONTEXT(%r5); // Update effective mode context
    beq-    L_ContextRestore;

    // Only bother saving r1, r2, r13+ as this function follows the ABI
    // This is, by the way, critical for the external interrupt handler to work
    stw     %r1, 0x004(%r4); // OSContext.gprs[1]
    stw     %r2, 0x008(%r4); // OSContext.gprs[2]
    mfcr    %r0;
    stmw    %r13, 0x034(%r4); // OSContext.gprs[13+]
    mflr    %r5;
    stw     %r0, 0x080(%r4); // OSContext.cr
    stw     %r5, 0x084(%r4); // OSContext.lr
    // mfctr   %r6;
    stw     %r5, 0x198(%r4); // OSContext.srr0
    mfxer   %r0;
    // stw     %r6, 0x088(%r4); // OSContext.ctr
    // mffs    %r5;
    stw     %r0, 0x090(%r4); // OSContext.xer
    mfmsr   %r0;
    // stw     %r6, 0x194(%r4); // OSContext.fpscr
    mfspr   %r5, GQR0;
    stw     %r0, 0x19C(%r4); // OSContext.srr1
    mfspr   %r6, GQR1;
    stw     %r5, 0x1A4(%r4); // OSContext.gqrs[0]
    mfspr   %r0, GQR2;
    stw     %r6, 0x1A8(%r4); // OSContext.gqrs[1]
    mfspr   %r5, GQR3;
    stw     %r0, 0x1AC(%r4); // OSContext.gqrs[2]
    mfspr   %r6, GQR4;
    stw     %r5, 0x1B0(%r4); // OSContext.gqrs[3]
    mfspr   %r0, GQR5;
    stw     %r6, 0x1B4(%r4); // OSContext.gqrs[4]
    mfspr   %r5, GQR6;
    stw     %r0, 0x1B8(%r4); // OSContext.gqrs[5]
    mfspr   %r6, GQR7;
    stw     %r5, 0x1BC(%r4); // OSContext.gqrs[6]
    stw     %r6, 0x1C0(%r4); // OSContext.gqrs[7]

L_ContextSwitch_Restore:;
    b       L_ContextRestore;
                  // clang-format on
);

} // namespace wsh::ppc