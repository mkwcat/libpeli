// wsh/runtime/Crt0.cpp - Initial program init
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "../common/Asm.h"
#include "../common/Types.h"
#include "../ppc/Bat.hpp"
#include "../ppc/Cache.hpp"
#include "../ppc/Hid0.hpp"
#include "../ppc/Hid4.hpp"
#include "../ppc/L2Cache.hpp"
#include "../ppc/Msr.hpp"
#include "../ppc/Ps.hpp"
#include "../ppc/Sync.hpp"
#include "Args.hpp"
#include "Exception.hpp"
#include "Memory.hpp"
#include "Thread.hpp"
#include <algorithm>

namespace wsh::runtime {

extern "C" Args _Wsh_HbLoadArg;
extern "C" void _start() noexcept;
extern "C" void _Wsh_Crt0() noexcept;

const Args *Crt0SystemArgv asm("__system_argv") = &_Wsh_HbLoadArg;

WSH_ASM_FUNCTION( // clang-format off
  [[gnu::weak]]
  void _start() noexcept,

  bl      _Wsh_Crt0;

  // TODO: Defining symbols this way isn't portable (requires GCC)
  .global _Wsh_HbLoadArg;
_Wsh_HbLoadArg:;
  .ascii  "_arg"; // Indicator
  .long   0;      // Magic
  .long   0;      // Command Line
  .long   0;      // Command Line Length
  .long   0;      // Argc
  .long   0;      // Argv
  .long   0;      // Argv End
                  // clang-format on
);

#ifndef WSH_CRT0_STACK_SIZE
#define WSH_CRT0_STACK_SIZE 0x8000
#endif

[[gnu::aligned(32)]]
static u8 Crt0Stack[WSH_CRT0_STACK_SIZE] asm("_Wsh_Crt0Stack");

WSH_ASM_FUNCTION( // clang-format off
  [[gnu::weak]]
  void _Wsh_Crt0() noexcept,

  // Set temporary stack pointer
  lis     %r1, (_Wsh_Crt0Stack + WSH_CRT0_STACK_SIZE - 0x8)@ha;
  la      %r1, (_Wsh_Crt0Stack + WSH_CRT0_STACK_SIZE - 0x8)@l(%r1);

  // Set the read-only Small Data Area 2 (SDA2) base pointer
  lis     %r2, _SDA2_BASE_@ha;
  la      %r2, _SDA2_BASE_@l(%r2);

  // Set the read-write Small Data Area (SDA) base pointer
  lis     %r13, _SDA_BASE_@ha;
  la      %r13, _SDA_BASE_@l(%r13);

  // Set GQR0 as it may be used by the compiler
  li      %r0, 0;
  mtspr   912, %r0;
                 
  // Call the main init function
  bl      _Wsh_Init;
                  // clang-format on
);

static void InitPowerPC() {
  // These values are set by IOS. Note that IOS versions after IOS28 expanded
  // the size of MEM2 addressable to PPC. In the future we should probably
  // select the MEM2 size based on the expected IOS version / the IOS version
  // we'll launch later, rather than whatever we're launched with.
  // u32 mem1Size = ios::gLoMem.osGlobals.mem1End - 0x80000000;
  // u32 mem2Size = ios::gLoMem.osGlobals.mem2End - 0x90000000;

  u32 mem1Size = 0x01800000u;
  u32 mem2Size = 0x03600000u;

  // Switch to Real Mode
  {
    ppc::Msr::RealModeScope scope;

    // Make sure the rms variable is stored before we store the cache
    asm volatile("" : : "m"(scope.msr));

    // Store temporary stack before flash invalidate
    if (ppc::Hid0::MoveFrom().DCE) {
      ppc::Cache::DcStore<false>(&Crt0Stack - 0x80000000, sizeof(Crt0Stack));
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
        .BHT = true,  // Branch History Table
    });

    // HID4 = 0x82000000
    ppc::Hid4::MoveTo(ppc::Hid4Bits{
        .H4A = true, // HID4 Access
        .SBE = true, // Secondary BAT Enable
    });
    ppc::ISync();

    // Configure the Block Address Translation registers
    ppc::BatConfig(std::max(0x01800000u, mem1Size),
                   std::max(0x03400000u, mem2Size), false, false, false);

    // Set MSR before leaving real mode
    scope.msr = ppc::Msr(ppc::MsrBits{
        .EE = 0, // External Interrupt Disabled
        .FP = 1, // Floating Point Available
        .ME = 0, // Machine Check Disabled
        .IR = 1, // Enable Instruction Address Translation
        .DR = 1, // Enable Data Address Translation
        .RI = 0, // Interrupt is not recoverable
    });
  }

  StubExceptionHandlers();

  // Init Paired Singles
  ppc::Ps::Init();

  // Init and enable L2 cache
  ppc::L2Cache::Init();
}

extern "C" int main(int argc, char **argv);

extern "C" [[gnu::used]]
void _Wsh_Init() {
  InitPowerPC();
  Memory::InitArena();

  // TODO: Building arguments this way results in writing to the .text section!
  // This may not be always possible.
  _Wsh_HbLoadArg.Build();

  // Initialize the thread system
  Thread::SystemInit(Crt0Stack, WSH_CRT0_STACK_SIZE);

  InitExceptions();

  main(_Wsh_HbLoadArg.argc, _Wsh_HbLoadArg.argv);
}

} // namespace wsh::runtime