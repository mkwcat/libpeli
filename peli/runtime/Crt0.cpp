// peli/runtime/Crt0.cpp - Initial program init
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "../cmn/Asm.h"
#include "../cmn/AsmRegisters.h"
#include "../cmn/Macro.h"
#include "../cmn/Types.hpp"
#include "../host/Config.h"
#include "../ios/low/Ipc.hpp"
#include "../ppc/Bat.hpp"
#include "../ppc/Cache.hpp"
#include "../ppc/Hid0.hpp"
#include "../ppc/Hid4.hpp"
#include "../ppc/L2Cache.hpp"
#include "../ppc/Msr.hpp"
#include "../ppc/PairedSingle.hpp"
#include "../ppc/SprRwCtl.hpp"
#include "../ppc/Sync.hpp"
#include "../util/Address.hpp"
#include "../util/Bit.hpp"
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

  // Workaround for HBC loader_reloc.c:27 patch_crt0
  .long   0x40000000;
                 // clang-format on
);
} // extern "C"

namespace {

constinit Args s_args;

alignas(32) u8 s_crt0_stack[PELI_CRT0_STACK_SIZE];

constexpr u32 s_hid0_default = util::BitCast<u32>(ppc::Hid0Bits{
    .DOZE = true,  // Doze Mode Available
    .DPM = true,   // Dynamic Power Management
    .NHR = true,   // Not Hard Reset
    .ICE = true,   // Instruction Cache Enable
    .DCE = true,   // Data Cache Enable
    .ICFI = true,  // Instruction Cache Flash Invalidate
    .DCFI = false, // Not Data Cache Flash Invalidate
    .SGE = false,  // Disable Speculation Guard
    .DCFA = false, // Data Cache Flush Assist
    .BTIC = true,  // Branch Target Instruction Cache
    .ABE = false,  // Address Broadcast Disabled
    .BHT = true,   // Branch History Table
});

constexpr u32 s_hid0_dcfi = util::BitCast<u32>(ppc::Hid0Bits{
    .DCFI = true, // Data Cache Flash Invalidate
});

constexpr u32 s_hid4_default = util::BitCast<u32>(ppc::Hid4Bits{
    .H4A = true, // HID4 Access
    .SBE = true, // Secondary BAT Enable
});

constexpr u32 s_msr_default = util::BitCast<u32>(ppc::MsrBits{
    .IR = 1, // Enable Instruction Address Translation
    .DR = 1, // Enable Data Address Translation
});

// Use constants for MEM1 and MEM2 sizes
constexpr u32 s_mem1_size = 0x01800000u;
constexpr u32 s_mem2_size = 0x03400000u;

constexpr ppc::BatConfig s_bats_default =
    ppc::BatConfig(s_mem1_size, s_mem2_size, false, false, false);

PELI_ASM_METHOD( // clang-format off
  void peliCrt0() noexcept,

  (PELI_ASM_IMPORT(i, s_crt0_stack),
   PELI_ASM_IMPORT(i, peliMain), 
   PELI_ASM_IMPORT(i, clearMemory),

   PELI_ASM_IMPORT(i, s_hid0_default),
   PELI_ASM_IMPORT(i, s_hid0_dcfi),
   PELI_ASM_IMPORT(i, s_hid4_default),
   PELI_ASM_IMPORT(i, s_msr_default),

   PELI_ASM_IMPORT_AS(i, ppc::Msr::EnterRealMode, EnterRealMode),
   PELI_ASM_IMPORT_AS(i, ppc::Msr::ExitRealMode, ExitRealMode),
   PELI_ASM_IMPORT_AS(i, ppc::BatClearAll, BatClearAll),
   PELI_ASM_IMPORT_AS(i, ppc::BatConfigure<s_bats_default>, BatConfigure),
   PELI_ASM_IMPORT_AS(i, ppc::L2Cache::Init, L2CacheInit),
   PELI_ASM_IMPORT(i, StubExceptionHandlers)
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

  // Load the return address this way as we don't know what kind of address
  // mapping we're running in right now
  lis     r3, .L%=ReturnFromEnterRealMode@ha;
  la      r3, .L%=ReturnFromEnterRealMode@l(r3);
  mtlr    r3;
  b       %[EnterRealMode];
.L%=ReturnFromEnterRealMode:;
  // { // Begin real mode

  // Set default HID0
  lis     r4, %[s_hid0_default]@h;
  ori     r4, r4, %[s_hid0_default]@l;
  // Enable data cache flash invalidate only if data cache is enabled
  mfspr   r0, PELI_SPR_HID0;
  rlwimi  r0, r0, 31 - 4, %[s_hid0_dcfi]; // Move DCE to DCFI and insert
  mtspr   PELI_SPR_HID0, r4;

  // Set default HID4
  lis     r4, %[s_hid4_default]@h;
  ori     r4, r4, %[s_hid4_default]@l;
  mtspr   PELI_SPR_HID4, r4;

  isync;

  bl      %[BatClearAll];
  bl      %[BatConfigure];

  li      r3, %[s_msr_default];
  bl      %[ExitRealMode];
  // } // End real mode

  bl      %[StubExceptionHandlers];
  bl      %[L2CacheInit];

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
  // r8 = size word aligned down

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

void peliMain(Args *input_args) noexcept {
  // Init Paired Singles
#if defined(PELI_ENABLE_PAIRED_SINGLE)
  ppc::PairedSingle::Init();
#else
  ppc::PairedSingle::Disable();
#endif

  Memory::InitArena();

  s_args.Build(input_args);

  // Initialize the thread system
  Thread::SystemInit(s_crt0_stack, PELI_CRT0_STACK_SIZE);

  // Initialize exception handlers
  InitExceptions();

  // Initialize Inter-process Communication with IOS
  ios::low::Init();

  _PELI_DIAGNOSTIC(push)
  // Ignore the warning on use of ::main
  _PELI_DIAGNOSTIC(ignored "-Wpedantic")
  ::exit(::main(s_args.argc, s_args.argv));
  _PELI_DIAGNOSTIC(pop)
}

} // namespace
} // namespace peli::runtime

constinit peli::runtime::Args *__system_argv = &peli::runtime::s_args;
