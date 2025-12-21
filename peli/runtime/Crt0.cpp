// peli/runtime/Crt0.cpp - Initial program init
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "../common/Asm.h"
#include "../common/AsmRegisters.h"
#include "../common/Macro.h"
#include "../common/Types.hpp"
#include "../host/Config.h"
#include "../ppc/Bat.hpp"
#include "../ppc/Cache.hpp"
#include "../ppc/Hid0.hpp"
#include "../ppc/Hid4.hpp"
#include "../ppc/L2Cache.hpp"
#include "../ppc/Msr.hpp"
#include "../ppc/Ps.hpp"
#include "../ppc/SprRwCtl.hpp"
#include "../ppc/Sync.hpp"
#include "../util/Address.hpp"
#include "Args.hpp"
#include "Exception.hpp"
#include "Memory.hpp"
#include "Thread.hpp"

#ifndef PELI_CRT0_STACK_SIZE
#define PELI_CRT0_STACK_SIZE 0x8000
#endif

int main(int argc, char **argv);
extern "C" [[gnu::noreturn]] void exit(int status);

extern "C" peli::runtime::Args *__system_argv;

namespace peli::runtime {

namespace {
void peliMain(Args *input_args) noexcept;
void peliCrt0() noexcept;
void clearMemory(void *start, u32 size) noexcept;
void initPowerPC();
} // namespace

extern "C" {
PELI_ASM_METHOD( // clang-format off
  [[gnu::weak]]
  void _start() noexcept,

  (PELI_ASM_IMPORT(i, peliCrt0)),

  bl      %[peliCrt0];

  // Homebrew loader arguments
  .ascii  "_arg"; // Indicator
  .long   0;      // Magic
  .long   0;      // Command Line
  .long   0;      // Command Line Length
  .long   0;      // Argc
  .long   0;      // Argv
  .long   0;      // Argv End
                 // clang-format on
);
}

namespace {

constinit Args s_args;

alignas(32) u8 s_crt0_stack[PELI_CRT0_STACK_SIZE];

PELI_ASM_METHOD( // clang-format off
  void peliCrt0() noexcept,

  (PELI_ASM_IMPORT(i, s_crt0_stack), PELI_ASM_IMPORT(i, peliMain), 
   PELI_ASM_IMPORT(i, clearMemory), PELI_ASM_IMPORT(i, initPowerPC)
  ),

  // Set stack pointer
  lis     r1, (%[s_crt0_stack] + PELI_CRT0_STACK_SIZE - 0x8)@ha;
  la      r1, (%[s_crt0_stack] + PELI_CRT0_STACK_SIZE - 0x8)@l(r1);

  // Set the read-only Small Data Area 2 (SDA2) base pointer
  .extern _SDA2_BASE_;
  lis     r2, _SDA2_BASE_@ha;
  la      r2, _SDA2_BASE_@l(r2);

  // Set the read-write Small Data Area (SDA) base pointer
  .extern _SDA_BASE_;
  lis     r13, _SDA_BASE_@ha;
  la      r13, _SDA_BASE_@l(r13);

  // Set GQR0 as it may be used by the compiler
  li      r0, 0;
  mtspr   PELI_SPR_GQR0, r0;

  // Load the return address this way as we don't know what kind of address
  // mapping we're running in right now
  lis     r3, .L%=ReturnFromInitPPC@ha;
  la      r3, .L%=ReturnFromInitPPC@l(r3);
  mtlr    r3;
  b       %[initPowerPC];
.L%=ReturnFromInitPPC:;

  // Clear bss before we use the stack again
  lis     r3, __bss_start@ha;
  la      r3, __bss_start@l(r3);
  lis     r4, __bss_end@ha;
  la      r4, __bss_end@l(r4);
  sub     r4, r3, r4;
  bl      %[clearMemory];

  lis     r3, __sbss_start@ha;
  la      r3, __sbss_start@l(r3);
  lis     r4, __sbss_end@ha;
  la      r4, __sbss_end@l(r4);
  sub     r4, r3, r4;
  bl      %[clearMemory];

  // Set first parameter to homebrew loader args
  lis     r3, (_start + 4)@ha;
  la      r3, (_start + 4)@l(r3);

  // Call the main init function
  bl      %[peliMain];
                 // clang-format on
);

PELI_ASM_METHOD( // clang-format off
  void clearMemory(void* start, u32 size) noexcept,
  (),

  // clang-format off
  li      r5, 0;
  cmpwi   r4, 4;
  blt-    .L%=VeryShort;

  // Align up to 4 bytes in case it's misaligned
  rlwinm. r6, r3, 0, 3;
  beq+    .L%=PreWordAligned;
  cmpwi   r6, 2;
  subfic  r6, r6, 4;
  stb     r5, 0(r3);
  bgt-    .L%=PreWordAlignDone;
  stb     r5, 1(r3);
  beq+    .L%=PreWordAlignDone;
  stb     r5, 2(r3);
.L%=PreWordAlignDone:;
  sub     r4, r4, r6;

.L%=PreWordAligned:;
  add     r6, r3, r6;

  // Cache line stop boundary
  addi    r7, r6, 31;
  rlwinm  r9, r7, 0, ~31;
  // r9 = align_up(r6, 32)
  subc.   r7, r9, r6;
  // r7 = distance to next cache line boundary

  rlwinm  r8, r4, 0, ~3;
  // r8 = n word aligned down

  cmplw   cr7, r7, r8;
  blt-    cr7, .L%=PreWordMinBoundary;
  mr.     r7, r8;
  // r7 = min(r7, r8)

.L%=PreWordMinBoundary:;
  beq-    .L%=CacheAligned;

  srwi    r8, r7, 2;
  mtctr   r8;
  stw     r5, 0x0(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0x4(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0x8(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0xC(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0x10(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0x14(r6);
  bdz-    .L%=PreWordOut;
  stw     r5, 0x18(r6);
.L%=PreWordOut:;
  sub     r4, r4, r7;

.L%=CacheAligned:;
  rlwinm. r7, r4, 0, ~31;
  beq+    .L%=PostWordAligned;

  add     r6, r7, r9;
  // r6 = align_down(r3 + n, 32)
  sub     r4, r4, r7;
  // r4 = amount left after cache line finishes

.L%=CacheAlignedZeroLoop:;
  subic.  r7, r7, 32;
  dcbz    r7, r9;
  bgt+    .L%=CacheAlignedZeroLoop;

.L%=PostWordAligned:;
  rlwinm. r7, r4, 0, ~3;
  beq-    .L%=PostWordOut;

  cmplwi  r7, 0x8;
  stw     r5, 0x0(r6);
  blt-    .L%=PostWordOut;
  stw     r5, 0x4(r6);
  beq-    .L%=PostWordOut;
  stw     r5, 0x8(r6);
  cmpwi   r7, 0x8+0x8;
  blt-    .L%=PostWordOut;
  stw     r5, 0xC(r6);
  beq-    .L%=PostWordOut;
  stw     r5, 0x10(r6);
  cmpwi   r7, 0x10+0x8;
  blt-    .L%=PostWordOut;
  stw     r5, 0x14(r6);
  beq-    .L%=PostWordOut;
  stw     r5, 0x18(r6);

.L%=PostWordOut:;
  sub.    r4, r4, r7;
  beqlr-;

  add     r6, r6, r4;
  cmpwi   r4, 2;
  stb     r5, -1(r6);
  bltlr-;
  stb     r5, -2(r6);
  beqlr+;
  stb     r5, -3(r6);
  blr;

.L%=VeryShort:;
  cmpwi   r5, 0;
  beqlr-;

  cmpwi   r5, 2;
  stb     r4, 0(r3);
  bltlr-;
  stb     r4, 1(r3);
  beqlr+;
  stb     r4, 2(r3);
  blr;
                 // clang-format on
);

[[gnu::noinline]] [[gnu::optimize("-Os")]] [[gnu::optimize(
    "-fno-schedule-insns")]]
void initPowerPC() {
  // Use constants for MEM1 and MEM2 sizes
  constexpr u32 mem1_size = 0x01800000u;
  constexpr u32 mem2_size = 0x03400000u;

  // Switch to Real Mode
  {
    ppc::Msr::RealModeScope scope;

    // Store temporary stack before flash invalidate
    if (ppc::Hid0::MoveFrom().DCE) {
      ppc::Cache::DcStore(util::Physical(s_crt0_stack), sizeof(s_crt0_stack));
      ppc::Sync();
      ppc::SprRwCtl<ppc::Hid0>()->ABE = true; // Address Broadcast Enable
      ppc::ISync();
      ppc::Sync();
    }

    // HID0 = 0x00910C64
    ppc::Hid0::MoveTo(ppc::Hid0Bits{
        .DOZE = true, // Doze Mode Available
        .DPM = true,  // Dynamic Power Management
        .NHR = true,  // Not Hard Reset
        .ICE = true,  // Instruction Cache Enable
        .DCE = true,  // Data Cache Enable
        .ICFI = true, // Instruction Cache Flash Invalidate
        .DCFI = true, // Data Cache Flash Invalidate
        .SGE = false, // Disable Speculation Guard
        .DCFA = true, // Data Cache Flush Assist
        .BTIC = true, // Branch Target Instruction Cache
        .ABE = false, // Address Broadcast Disabled
        .BHT = true,  // Branch History Table
    });

    // HID4 = 0x82000000
    ppc::Hid4::MoveTo(ppc::Hid4Bits{
        .H4A = true, // HID4 Access
        .SBE = true, // Secondary BAT Enable
    });
    ppc::ISync();

    // Configure the Block Address Translation registers
    ppc::ConfigureBats<ppc::BatConfig(mem1_size, mem2_size, false, false,
                                      false)>();

    // Set MSR before leaving real mode
    scope.msr = ppc::Msr(ppc::MsrBits{
        .EE = 0, // External Interrupt Disabled
        .FP = 0, // Floating Point Unavailable
        .ME = 0, // Machine Check Disabled
        .IR = 1, // Enable Instruction Address Translation
        .DR = 1, // Enable Data Address Translation
        .RI = 0, // Interrupt is not recoverable
    });
  }

  // StubExceptionHandlers();

  // Init Paired Singles
#if defined(PELI_ENABLE_PAIRED_SINGLE)
  ppc::Ps::Init();
#else
  ppc::Ps::Disable();
#endif

  // Init and enable L2 cache
  ppc::L2Cache::Init();
}

void peliMain(Args *input_args) noexcept {
  Memory::InitArena();

  s_args.Build(input_args);

  // Initialize the thread system
  Thread::SystemInit(s_crt0_stack, PELI_CRT0_STACK_SIZE);

  InitExceptions();

  _PELI_DIAGNOSTIC(push)
  // Ignore the warning on use of ::main
  _PELI_DIAGNOSTIC(ignored "-Wpedantic")
  ::exit(::main(s_args.argc, s_args.argv));
  _PELI_DIAGNOSTIC(pop)
}

} // namespace
} // namespace peli::runtime

constinit peli::runtime::Args *__system_argv = &peli::runtime::s_args;
