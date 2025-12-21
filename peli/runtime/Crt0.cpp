// peli/runtime/Crt0.cpp - Initial program init
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "../common/Asm.h"
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

extern "C" _PELI_GNU_CLANG_ONLY([[gnu::noreturn]]) void exit(int status);

extern "C" peli::runtime::Args *__system_argv;

namespace peli::runtime {

// Make the entire file extern "C" for easier accessing from assembly.
extern "C" {

PELI_ASM_FUNCTION( // clang-format off
  [[gnu::weak]]
  void _start() noexcept,

  bl      peliCrt0;

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

namespace {

constinit Args s_args;
alignas(32) u8 s_crt0_stack[PELI_CRT0_STACK_SIZE];

void peliMain(Args *input_args) noexcept;
void peliCrt0() noexcept;

PELI_ASM_FUNCTION( // clang-format off
  void peliCrt0() noexcept,

  // Set temporary stack pointer
  lis     %r1, (s_crt0_stack + PELI_CRT0_STACK_SIZE - 0x8)@ha;
  la      %r1, (s_crt0_stack + PELI_CRT0_STACK_SIZE - 0x8)@l(%r1);

  // Set the read-only Small Data Area 2 (SDA2) base pointer
  lis     %r2, _SDA2_BASE_@ha;
  la      %r2, _SDA2_BASE_@l(%r2);

  // Set the read-write Small Data Area (SDA) base pointer
  lis     %r13, _SDA_BASE_@ha;
  la      %r13, _SDA_BASE_@l(%r13);

  // Set GQR0 as it may be used by the compiler
  li      %r0, 0;
  mtspr   912, %r0;

  mflr    %r3;
                 
  // Call the main init function
  bl      peliMain;
                   // clang-format on
);

void initPowerPC() {
#if 0
  // These values are set by IOS. Note that IOS versions after IOS28 expanded
  // the size of MEM2 addressable to PPC. In the future we should probably
  // select the MEM2 size based on the expected IOS version or the IOS version
  // we'll launch later, rather than whatever we're launched with.
  u32 mem1_size = std::max(ios::g_lo_mem.os_globals.mem1_end - 0x80000000u, 0x01800000u);
  u32 mem2_size = std::max(ios::g_lo_mem.os_globals.mem2_end - 0x90000000u, 0x03400000u);
#else
  // For now, use constants for MEM1 and MEM2 sizes
  constexpr u32 mem1_size = 0x01800000u;
  constexpr u32 mem2_size = 0x03400000u;
#endif

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
    ppc::BatConfig(mem1_size, mem2_size, false, false, false);

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

  StubExceptionHandlers();

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
  initPowerPC();

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
} // extern "C"
} // namespace peli::runtime

constinit peli::runtime::Args *__system_argv = &peli::runtime::s_args;
